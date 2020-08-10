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

//Initial setup of the chip
void IMU::init() {
	//Device and signal path reset as per instructions on p41 of MPU6000 register map
	write8To(MPUREG_PWR_MGMT_1, BIT_H_RESET);
	usleep(100000);	//100ms
	write8To(MPUREG_SIGNAL_PATH_RESET, 0x7); //0x7 = 00000111, gyro, accel and temp reset
	usleep(100000);	//100ms

	//Disable I2C in favour of SPI interface (p38 of MPU6000 register map)
	write8To(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);

	//Set internal clock source to the Z-axis gyroscope
	//Use of a gyro clock reference is recommended for stability (p40 of MPU6000 register map)
	write8To(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
	
	//Disable interrupts (write 0x00 to interrupt enable register, p27 of MPU6000 register map)
	write8To(MPUREG_INT_ENABLE, 0x00);

	//Set sample rate divisor to one (max sample rate)
	//This determines the rate at which the sensor registers will get updated
	//Sample rate = gyroscope output rate/(1+SMPLRT_DIV) where gyro rate is 1kHz
	//See p12 of MPU6000 register map if wanting to change this for any reason
	write8To(MPUREG_SMPLRT_DIV, 0);

	//Set digital low-pass filtering (DLPF)
	//Config field is a 3-bit value from 0 to 6 (7 is reserved) - see p13 of MPU6000 register map
	//Options defined in terms of bandwidth from 5Hz (slower, more stable) to 260Hz (faster, noisier)
	//I've chosen 5Hz on the assumption that our robots aren't going to have faster dynamics than this
	write8To(MPUREG_CONFIG, BITS_DLPF_CFG_5HZ);

	//Set accelerometer measurement scale
	//2-bit value to select max g-force of +/- 2g, 4g, 8g or 16g (p15 MPU6000 register map)
	//Assuming we're mainly interested in robot orientation, 2g should be suitable
	write8To(MPUREG_ACCEL_CONFIG, BITS_FS_2G);

	//Set gyro measurement scale
	//2-bit value to select +/- 250, 500, 1000 or 2000 degrees/sec (p14 MPU6000 register map)
	//Difficult to guess a sensible default value, using 500dps, may need changing by the user
	write8To(MPUREG_GYRO_CONFIG, BITS_FS_500DPS);

	//Check whoami
	if (read8From(MPUREG_WHOAMI) != 0x68) {
		printf("Error: MPU6000 'whoami' register not reading correctly. SPI comms may not be working\n");
	}

}

/********************************Private functions***************************************/

//8-bit write to register over SPI
void IMU::write8To(uint8_t address, uint8_t data) {
	//Prepare two bytes to send
	uint8_t txBuffer[2];
	txBuffer[0] = address;
	txBuffer[1] = data;

	//Transmit
	wiringPiSPIDataRW(IMU_CS_NUM, txBuffer, 2);
}

//8-bit read from register over SPI
uint8_t IMU::read8From(uint8_t address) {
	//Prepare two bytes to send
	//First byte needs bit [4] set (0x80), to indicate a read operation
	//Second byte is a dummy, to shift the result out from the slave to master
	uint8_t txRxBuffer[2];
	txRxBuffer[0] = address | READ_FLAG;
	txRxBuffer[1] = 0x00;

	//Transmit/receive. Result overwritten into txRxBuffer.
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

	//Transmit/receive. Result overwritten into txRxBuffer.
	wiringPiSPIDataRW(IMU_CS_NUM, txRxBuffer, 3);

	//Result is shifted into the buffer from the right, MSB-first, so will be in locations [1] and [2]
	//High byte is in [1], low byte in [2]
	return txRxBuffer[1] << 8 | txRxBuffer[2];
}