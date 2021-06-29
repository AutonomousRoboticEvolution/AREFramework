/**
	@file SensorOrgan.cpp
	@brief Implementation of SensorOrgan methods.
	@author Mike Angus
*/

#include <iostream> // for cout debugging
#include "SensorOrgan.hpp"
#include <wiringPi.h> // for millis()

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
			return 65535;
		}
	}
    // did not timeout :)
	uint16_t first_byte=timeOfFlight->read8From(VL53L0X_RESULT_RANGE_STATUS + 10);
	uint16_t second_byte=timeOfFlight->read8();
	uint16_t range = first_byte<<8 | second_byte;
	timeOfFlight->write8To(VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);
	return range;
}


//Reads the infrared sensor
uint16_t SensorOrgan::readInfrared() {
	write8To(REQUEST_INFRARED_REGISTER,0x00);
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

//Useful for testing and checking the organ works, not intended for general use
void SensorOrgan::test()
{
    std::cout<<"Testing the sensor..."<<std::endl;
	
//    // keep taking readings and displaying:
//	for (int i=0;i<100;i++){
		uint16_t TOFvalue = readTimeOfFlight();
		uint16_t IRvalue = readInfrared();
		std::cout<<"IR: "<<IRvalue;
		std::cout<<"\tTOF: "<<TOFvalue<<std::endl;
//		usleep(500000);
//	}

    //flashIndicatorLED(3);

    // testing the time-of-flight calibration
    #define number_of_ranges_to_test 25
    #define starting_distance 100
    #define distance_between_each_range 100
    #define number_of_readings_at_each_range 40
    #define length_of_usleep_between_readings 200000
    std::cout<<"Range calibration testing"<<std::endl;
    int actual_distances [number_of_ranges_to_test] = { };
    uint16_t measured_values[number_of_ranges_to_test][number_of_readings_at_each_range];

    for(int i = 0; i < number_of_ranges_to_test; i++) {
        actual_distances[i] = starting_distance + i*distance_between_each_range;
        std::cout << i <<": set the sensor to be at "<<actual_distances[i]/10 <<"cm"<<std::endl;
        std::cout<< "press enter when ready"<<std::endl;
        std::cin.get();
        std::cout<< "taking readings, stay still!"<<std::endl;
        for(int j=0; j<number_of_readings_at_each_range; j++){
            usleep(length_of_usleep_between_readings);
            measured_values[i][j] = readTimeOfFlight();
            std::cout<<"TOF: "<<measured_values[i][j] <<std::endl;
        }
    }
    std::cout<<"Finished testing, data: "<<std::endl;
    std::cout<<"Actual,\tmeasured ("<<number_of_readings_at_each_range<<" values)"<<std::endl;
    for(int i = 0; i < number_of_ranges_to_test; i++) {
        std::cout<< actual_distances[i];
            for(int j=0; j<number_of_readings_at_each_range; j++){
                std::cout<<","<<(unsigned)measured_values[i][j];
            }
        std::cout<<std::endl;
    }

}
