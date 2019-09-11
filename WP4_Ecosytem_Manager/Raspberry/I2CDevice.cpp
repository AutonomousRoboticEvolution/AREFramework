#include "I2CDevice.hpp"

//devAddress must be the 7-bit version, omitting the R/W bit (LSB)
I2CDevice::I2CDevice(uint8_t address) {
	this->devAddress = address; //Set address

	//Set Linux filehandle for device
	if ((this->devHandle = wiringPiI2CSetup(address)) == -1) {
		printf("[I2CDevice %02X] Init failed: %s\n", devAddress, strerror(errno));
	}
}

//8-bit write, for when register not specified by subaddress
void I2CDevice::write8(uint8_t data) {
	//Attempt to write to the device, report if error
	if(wiringPiI2CWrite(devHandle, data) < 0) {
		printf("[I2CDevice %02X] 8-bit write failed: %s\n", devAddress, strerror(errno));
	}
}

//8-bit write to specific register
void I2CDevice::write8To(uint8_t subAddress, uint8_t data) {
	//Attempt to write to the device, report if error
	if(wiringPiI2CWriteReg8(devHandle, subAddress, data) < 0) {
		printf("[I2CDevice %02X] 8-bit write to %02X failed: %s\n", devAddress, subAddress, strerror(errno));
	}
}

//16-bit write to specific register
void I2CDevice::write16To(uint8_t subAddress, uint16_t data) {
	//Attempt to write to the device, report if error
	if(wiringPiI2CWriteReg16(devHandle, subAddress, data) < 0) {
		printf("[I2CDevice %02X] 16-bit write failed: %s\n", devAddress, strerror(errno));
	}
}

//8-bit read, for when register not specified by subaddress
int I2CDevice::read8() {
	//Attempt to read from the device, report if error
	int rxData = wiringPiI2CRead(devHandle);
	if(rxData < 0) {
		printf("[I2CDevice %02X] 8-bit read failed: %s\n", devAddress, strerror(errno));
	}
	return rxData;
}

//8-bit read from specific register
int I2CDevice::read8From(uint8_t subAddress) {
	//Attempt to read from the device, report if error
	int rxData = wiringPiI2CReadReg8(devHandle, subAddress);
	if(rxData < 0) {
		printf("[I2CDevice %02X] 8-bit read failed: %s\n", devAddress, strerror(errno));
	}
	return rxData;
}

//16-bit read from specific register
int I2CDevice::read16From(uint8_t subAddress) {
	//Attempt to read from the device, report if error
	int rxData = wiringPiI2CReadReg16(devHandle, subAddress);
	if(rxData < 0) {
		printf("[I2CDevice %02X] 16-bit read failed: %s\n", devAddress, strerror(errno));
	}
	return rxData;
}