/**
	@file SensorOrgan.cpp
	@brief Implementation of SensorOrgan methods.
	@author Mike Angus
*/

#include "SensorOrgan.hpp"

//I2CSwitch functions
//Constructor
I2CSwitch::I2CSwitch(uint8_t address) : I2CDevice(address) {
	//Just uses the I2CDevice constructor
}
//Opens the bus
void I2CSwitch::enableChannel() {
	//Write '1' to bit [0] of control register to enable channel
	//Send to device (no subaddress; write8 inherited from I2CDevice)
	write8(0x01);
}
//Closes the bus
void I2CSwitch::disableChannel() {
	//Write '0' to bit [0] of control register to disable channel
	//Send to device (no subaddress; write8 inherited from I2CDevice)
	write8(0x00);
}

//SensorOrgan functions
//Constructor; must call all sensors' constructors in the initialiser list
SensorOrgan::SensorOrgan(uint8_t switchAddress) 
	//Initialiser list for member sensors
	: i2cSwitch(switchAddress), 
	proximity(VCNL4040_ADDRESS),
	ambientLight(VCNL4040_ADDRESS),
	adcModule(ADC_MUDULE),
	color(COLORSENSOR_ADDRESS),
	tof(TOF_ADDRESS)
{

	//Set member variable address
	this->address = switchAddress;
}

//*****************************PROXIMITY SENSOR******************

//Initialises the proximity sensor
void SensorOrgan::initProximity() {
	i2cSwitch.enableChannel();
	proximity.init();
	i2cSwitch.disableChannel();
}

//Reads the proximity sensor
uint16_t SensorOrgan::readProximity() {
	//Enable the channel, read sensor, then close channel
	i2cSwitch.enableChannel();
	int reading = proximity.read();
	i2cSwitch.disableChannel();

	return reading;
}

//*****************************AMBIENT LIGHT SENSOR******************
//Initialises the sensor
void SensorOrgan::initAmbientLight() {
	i2cSwitch.enableChannel();
	ambientLight.init();
	i2cSwitch.disableChannel();
}

//Reads the ambient light value
uint16_t SensorOrgan::readAmbientLight() {
	i2cSwitch.enableChannel();
	uint16_t reading = ambientLight.readAmbient();
	i2cSwitch.disableChannel();
	return reading;
}

//Reads the white value
uint16_t SensorOrgan::readWhite() {
	i2cSwitch.enableChannel();
	uint16_t reading = ambientLight.readWhite();
	i2cSwitch.disableChannel();
	return reading;
}

//***************ADC**************************
//Initialises the ADC
void SensorOrgan :: ADCinit()
{
	i2cSwitch.enableChannel();
	adcModule.init();
	i2cSwitch.disableChannel();
}
//Reads the ADC data
uint16_t SensorOrgan :: ADCdata()
{
	i2cSwitch.enableChannel();
	uint16_t data = adcModule.ADCread();
	i2cSwitch.disableChannel();
	return data;
}


//************color sensor********************
//Initialises the color sensor
void SensorOrgan :: ColorSensorinit()
{
	i2cSwitch.enableChannel();
	color.init();
	i2cSwitch.disableChannel();
}
//Read the color sensor data
uint16_t SensorOrgan :: read_color_channel(uint8_t ch)
{
	i2cSwitch.enableChannel();
	uint16_t color_values = color.readcolor(ch);
	i2cSwitch.disableChannel();
	return color_values;
	
}

//***************TOF**************************
//Initialises the TOF
void SensorOrgan :: initTOF(int iChan, int bLongRange)
{
	i2cSwitch.enableChannel();
	tof.init(iChan, bLongRange);
	i2cSwitch.disableChannel();
}
//Reads the TOF data
int SensorOrgan :: readDistanceTOF()
{
	i2cSwitch.enableChannel();
	uint16_t data = tof.readDistance();
	i2cSwitch.disableChannel();
	return data;
}


