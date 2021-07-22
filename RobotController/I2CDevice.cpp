#include "I2CDevice.hpp"
#define DEBUG_PRINT true

//devAddress must be the 7-bit version, omitting the R/W bit (LSB)
I2CDevice::I2CDevice(uint8_t address) {
	this->devAddress = address; //Set address

	//Set Linux filehandle for device
	if ((this->devHandle = wiringPiI2CSetup(address)) == -1) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] Init failed: %s\n", devAddress, strerror(errno));
	}
}

//8-bit write, for when register not specified by subaddress
void I2CDevice::write8(uint8_t data) {
	//Attempt to write to the device, report if error
	if(wiringPiI2CWrite(devHandle, data) < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 8-bit write failed: %s\n", devAddress, strerror(errno));
	}
}

//8-bit write to specific register
void I2CDevice::write8To(uint8_t subAddress, uint8_t data) {
	//Attempt to write to the device, report if error
	if(wiringPiI2CWriteReg8(devHandle, subAddress, data) < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 8-bit write to %02X failed: %s\n", devAddress, subAddress, strerror(errno));
	}
}

//16-bit write to specific register, big-endian
void I2CDevice::write16To(uint8_t subAddress, uint16_t data) {
	//Attempt to write to the device, report if error
	if(wiringPiI2CWriteReg16(devHandle, subAddress, data) < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 16-bit write failed: %s\n", devAddress, strerror(errno));
	}
}

//16-bit write to specific register, little-endian format
void I2CDevice::writeLittleEndian16To(uint8_t subAddress, uint16_t data) {
	//Swap bytes. Low byte << 8 OR'd with high byte >> 8
	uint16_t littleEndian = (data && 0x00FF) << 8 | (data & 0xFF00) >> 8;
	//Attempt to write to the device, report if error
	if(wiringPiI2CWriteReg16(devHandle, subAddress, littleEndian) < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 16-bit write failed: %s\n", devAddress, strerror(errno));
	}
}

//8-bit read, for when register not specified by subaddress
int I2CDevice::read8() {
	//Attempt to read from the device, report if error
	int rxData = wiringPiI2CRead(devHandle);
	if(rxData < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 8-bit read failed: %s\n", devAddress, strerror(errno));
	}
	return rxData;
}

//8-bit read from specific register
int I2CDevice::read8From(uint8_t subAddress) {
	//Attempt to read from the device, report if error
	int rxData = wiringPiI2CReadReg8(devHandle, subAddress);
	if(rxData < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 8-bit read failed: %s\n", devAddress, strerror(errno));
	}
	return rxData;
}

//16-bit read from specific register
int I2CDevice::read16From(uint8_t subAddress) {
	//Attempt to read from the device, report if error
	int rxData = wiringPiI2CReadReg16(devHandle, subAddress);
	if(rxData < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 16-bit read failed: %s\n", devAddress, strerror(errno));
	}
	return rxData;
}

//16-bit read from specific register, little-endian format
int I2CDevice::readLittleEndian16From(uint8_t subAddress) {
	//Attempt to read from the device, report if error
	int rxData = wiringPiI2CReadReg16(devHandle, subAddress);
	if(rxData < 0) {
		if(DEBUG_PRINT) printf("[I2CDevice 0x%02X] 16-bit read failed: %s\n", devAddress, strerror(errno));
	}
	//Swap bytes. Low byte << 8 OR'd with high byte >> 8
	int byteSwappedRxData = (rxData && 0x00FF) << 8 | (rxData & 0xFF00) >> 8;
	return byteSwappedRxData;
}
