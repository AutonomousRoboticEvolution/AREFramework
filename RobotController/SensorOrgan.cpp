/**
	@file SensorOrgan.cpp
	@brief Implementation of SensorOrgan methods.
	@author Mike Angus
*/

#include <iostream> // for cout debugging
#include "SensorOrgan.hpp"
#include <wiringPi.h> // for millis()
#include <bitset>

// Time of flight sensor (aka VL53L0X) constructor
VL53L0X::VL53L0X(uint8_t address) : I2CDevice(address){} // just call i2c device constructor

//Main Sensor organ Constructor
SensorOrgan::SensorOrgan(uint8_t address) : I2CDevice(address){
    // the VL53L0X is also an I2CDevice, and is always set to have the address one higher than the sensor organ Arduino
    // The Arduino will hangle all the setup, init() and address change of the VL53L0X
    timeOfFlight = new VL53L0X(address+1);
}


// Returns a range reading in millimeters. Continuous mode should already be active, as it is set by the Arduino at boot.
// the registers used here were found from the Arduino library.
uint16_t SensorOrgan::readTimeOfFlight() {
	
	uint16_t timeout_start_ms = millis();
	while ((timeOfFlight->read8From(VL53L0X_RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
		if (TIMEOUT_WHEN_RANGEFINDING_MILLISECONDS > 0 && ((uint16_t)(millis() - timeout_start_ms) > TIMEOUT_WHEN_RANGEFINDING_MILLISECONDS)) {
            std::cout<<"ERROR: Timeout attempting to read the time-of-flight sensor"<<std::endl;
        	timeOfFlight->write8To(VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);
            return VALUE_TO_RETURN_ON_TIMEOUT;
		}
	}
    // did not timeout :)
    usleep(100000);
	uint16_t first_byte=timeOfFlight->read8From(VL53L0X_RESULT_RANGE_STATUS + 10);
	uint16_t second_byte=timeOfFlight->read8();
	uint16_t range = first_byte<<8 | second_byte;
//    std::cout<<"raw range: "<<range<<std::endl;
	timeOfFlight->write8To(VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);
    if (range>MAXIMUM_RANGE_VALUE) return MAXIMUM_RANGE_VALUE;
    else return range;

}


//Reads the infrared sensor
uint16_t SensorOrgan::readInfrared() {
	write8To(REQUEST_INFRARED_REGISTER,0x00);
    uint8_t first_byte = read8();
    uint8_t second_byte = read8();
	return (first_byte<<8) | second_byte;
}

//Reads the infrared sensor without any filtering etc
uint16_t SensorOrgan::readInfraredRaw() {
	write8To(REQUEST_INFRARED_RAW_VALUE_REGISTER,0x00);
    uint8_t first_byte = read8();
    uint8_t second_byte = read8();
	return (first_byte<<8) | second_byte;
}

void SensorOrgan::flashIndicatorLED(uint8_t numberOfFlashes) {
	write8To(FLASH_INDICATOR_LED_REGISTER,numberOfFlashes);
}

// tells the arduino to change the i2c address of the VL53L0X
void SensorOrgan::setTimOfFlightI2CAddress(uint8_t newAddress){
    write8To(SET_TIME_OF_FLIGHT_ADDRESS_REGISTER, newAddress);
}

uint16_t SensorOrgan::getTimeOfFlightI2CAddress(){
    return read8From(GET_TIME_OF_FLIGHT_ADDRESS_REGISTER);
}


void SensorOrgan::set_test_value(uint8_t value){
    write8To(SET_TEST_VALUE_REGISTER, value);
}
uint8_t SensorOrgan::get_test_value(){
    return read8From(GET_TEST_VALUE_REGISTER);
}

//Useful for testing and checking the organ works, not intended for general use
void SensorOrgan::test()
{
    std::cout<<"Testing the sensor..."<<std::endl;
    int value_to_send = 0b10101010 ;
    for (int i=0;i<10;i++){
        set_test_value(value_to_send);
        std::cout<<int(value_to_send)<< ",\t"<< int( get_test_value() ) << std::endl;
    }
/*
    // keep taking readings and displaying:
	for (int i=0;i<100;i++){
		uint16_t TOFvalue = readTimeOfFlight();
		std::bitset<8> TOFbits(TOFvalue);
		uint16_t IRvalue = readInfrared();
		std::bitset<8> IRbits(IRvalue);
		uint16_t IRraw = readInfraredRaw();
		std::bitset<8> IRRawbits(IRraw);

		std::cout<<"IR: "<<IRvalue<<"\t"<<IRbits<<std::endl;
		std::cout<<"TOF: "<<TOFvalue<<"\t"<<TOFbits<<std::endl;
		std::cout<<"IRraw: "<<IRraw<<"\t"<<IRRawbits<<std::endl;
		usleep(500000);
	}
*/

    //flashIndicatorLED(3);
	
	/*
    // testing the time-of-flight calibration
    #define number_of_ranges_to_test 15
    #define number_of_offsets_to_test 1
    #define starting_distance 100
    #define distance_between_each_range 100
    #define number_of_readings_at_each_range 50
    #define length_of_usleep_between_readings 30000
    std::cout<<"Range calibration testing"<<std::endl;
    int actual_distances [number_of_ranges_to_test] = { };
    uint16_t measured_values_TOF[number_of_ranges_to_test][number_of_offsets_to_test][number_of_readings_at_each_range];
    uint16_t measured_values_IR[number_of_ranges_to_test][number_of_offsets_to_test][number_of_readings_at_each_range];

    for(int i_offset = 0; i_offset < number_of_offsets_to_test; i_offset++) {
        std::cout<<"Set sideways offset of: "<<i_offset*distance_between_each_range<<std::endl;
        for(int i = 0; i < number_of_ranges_to_test; i++) {
            actual_distances[i] = starting_distance + i*distance_between_each_range;
            std::cout << i <<": set the sensor to be at "<<actual_distances[i]/10 <<"cm"<<std::endl;
            std::cout<< "press enter when ready"<<std::endl;
            std::cin.get();
            std::cout<< "taking readings, stay still!"<<std::endl;
            for(int j=0; j<number_of_readings_at_each_range; j++){
                usleep(length_of_usleep_between_readings + rand()%length_of_usleep_between_readings);
                measured_values_TOF[i][i_offset][j] = readTimeOfFlight();
//                std::cout<<measured_values_TOF[i][i_offset][j]<<std::endl;
                usleep(100);
                measured_values_IR[i][i_offset][j] = readInfrared();
            }
        }
    }
    std::cout<<"\n\nTime of flight sensor:\n: "<<std::endl;
    std::cout<<"Actual,\tmeasured ("<<number_of_readings_at_each_range<<" values)"<<std::endl;
    for(int i_offset = 0; i_offset < number_of_offsets_to_test; i_offset++) {
        std::cout<<"Sideways offset: "<<i_offset*distance_between_each_range<<std::endl;
        for(int i = 0; i < number_of_ranges_to_test; i++) {
            std::cout<< actual_distances[i];
                for(int j=0; j<number_of_readings_at_each_range; j++){
                    uint16_t temp = measured_values_TOF[i][i_offset][j];
                    std::cout<<","<< temp ;
                }
            std::cout<<std::endl;
        }
    }
    std::cout<<"\n\nInfrared:\n: "<<std::endl;
    for(int i_offset = 0; i_offset < number_of_offsets_to_test; i_offset++) {
        std::cout<<"Sideways offset: "<<i_offset*distance_between_each_range<<std::endl;
        for(int i = 0; i < number_of_ranges_to_test; i++) {
            std::cout<< actual_distances[i];
                for(int j=0; j<number_of_readings_at_each_range; j++){
                    std::cout<<","<< measured_values_IR[i][i_offset][j];
                }
            std::cout<<std::endl;
        }
    }
	*/

}
