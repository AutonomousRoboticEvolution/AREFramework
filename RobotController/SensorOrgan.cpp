/**
	@file SensorOrgan.cpp
	@brief Implementation of SensorOrgan methods.
	@author Mike Angus
*/

#include "SensorOrgan.hpp"


// Time of flight sensor (aka VL53L0X) constructor
VL53L0X::VL53L0X(uint8_t address) : I2CDevice(address){} // just call i2c device constructor

//Main Sensor organ Constructor
SensorOrgan::SensorOrgan(uint8_t address) : Organ(address){
    // record organ type:
    organType = SENSOR;

    // the VL53L0X is also an I2CDevice, and is always set to have the address one higher than the sensor organ Arduino
    // The Arduino will handle all the setup, init() and address change of the VL53L0X
    timeOfFlight = new VL53L0X(address+1);


    // Tell arduino what the the IR threashold is.
    // Only used for indicator LED, since the threasholding is done on the pi side - see readInfraredNormalised() method below.
    // If you comment out this line, the indicator LED on the sensor will show a PWM value corresponding to the IR reading.
    setInfraredThreashold(INFRARED_SENSOR_DEFAULT_THREASHOLD);
}

// Returns a range reading in millimeters. Continuous mode should already be active, as it is set by the Arduino at boot.
// the registers used here were found from the Arduino VL53L0 library.
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
	uint16_t first_byte=timeOfFlight->read8From(VL53L0X_RESULT_RANGE_STATUS + 10);
	uint16_t second_byte=timeOfFlight->read8();
	uint16_t range = first_byte<<8 | second_byte;
//    std::cout<<"raw range: "<<range<<std::endl;
	timeOfFlight->write8To(VL53L0X_SYSTEM_INTERRUPT_CLEAR, 0x01);
    if (range>MAXIMUM_RANGE_VALUE) return MAXIMUM_RANGE_VALUE;
    else return range;

}

// return the time of flight as a value between 0 and 1
float SensorOrgan::readTimeOfFlightNormalised(){
    return std::max ( std::min(1.0,float( this->readTimeOfFlight() )/NORMALISE_TIME_OF_FLIGHT_MAX) , 0.0);
}

//Reads the infrared sensor
uint8_t SensorOrgan::readInfrared() {
	write8To(REQUEST_INFRARED_REGISTER,0x00);
    return read8();
}

// return the time of flight as a value between 0 and 1
float SensorOrgan::readInfraredNormalised(){
    if (this->readInfrared() > infraredSensorThreashold) { return 1.0; }
    else {return 0.0;}
}

void SensorOrgan::setInfraredThreashold(uint8_t new_value){
    infraredSensorThreashold = new_value;
    write8To(SET_INFRARED_THREASHOLD_REGISTER , new_value);
}

//Reads the infrared sensor without any filtering etc
uint16_t SensorOrgan::readInfraredRaw() {
	write8To(REQUEST_INFRARED_RAW_VALUE_REGISTER,0x00);
    uint8_t first_byte = read8();
    uint8_t second_byte = read8();
    //int getOrganI2CAddress(){ return getI2CAddress();}
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

