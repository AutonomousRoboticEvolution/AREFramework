/**
	@file IMU.cpp
	@brief Functions for the MPU6000 accelerometer/gyro chip
	Some elements loosely based on MPU6000_spi library for Mbed by Bruno Alfano
	https://os.mbed.com/users/brunoalfano/code/MPU6000_spi/
	@author Mike Angus
*/

#include "IMU.hpp"

//Constructor, sets up the SPI device
IMU::IMU() {
	if (wiringPiSPISetup(IMU_CS_NUM, SPI_FREQ_HZ) < 0) {
		printf("Error initialising SPI with csNum %d. Errno: %d\n", IMU_CS_NUM, errno);
	}
}

/********************************Private functions***************************************/

//8-bit write to register over SPI
void IMU::write8To(uint8_t address, uint8_t data) {
	//Prepare two bytes to send
	uint8_t sendBuffer[2];
	sendBuffer[0] = address;
	sendBuffer[1] = data;

	//Transmit
	wiringPiSPIDataRW(IMU_CS_NUM, sendBuffer, 2);
}

//8-bit read from register over SPI
uint8_t IMU::read8From(uint8_t address) {
	//Prepare two bytes to send
	//First byte needs bit [4] set (0x80), to indicate a read operation
	//Second byte is a dummy, to shift the result out from the slave to master
	uint8_t txRxBuffer[2];
	txRxBuffer[0] = address | READ_FLAG;
	txRxBuffer[1] = 0x00;

	//Transmit
	wiringPiSPIDataRW(IMU_CS_NUM, txRxBuffer, 2);

	//Result is shifted into the buffer from the right, so will be in location [1]
	return	txRxBuffer[1];
}

//16-bit read from register over SPI
uint16_t IMU::read16From(uint8_t address) {
	//Prepare three bytes to send
	//First byte needs bit [4] set (0x80), to indicate a read operation
	//2nd/3rd bytes are dummies, to shift the 16-bit result out from the slave to master
	uint8_t txRxBuffer[3];
	txRxBuffer[0] = address | READ_FLAG;
	txRxBuffer[1] = 0x00;
	txRxBuffer[2] = 0x00;

	//Transmit
	wiringPiSPIDataRW(IMU_CS_NUM, txRxBuffer, 3);

	//Result is shifted into the buffer from the right, MSB-first, so will be in locations [1] and [2]
	//Pointer-to-8bit address of high byte can be cast to a pointer-to-16bit, 
	//and dereferenced to get 16bit result by joining two bytes from [1] and [2]
	return *(uint16_t*)&txRxBuffer[1];;
}