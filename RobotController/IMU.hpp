/**
	@file IMU.hpp
	@brief Header and documentation for IMU.cpp, for the MPU6000 accelerometer/gyro chip
	Some elements based on MPU6000_spi library for Mbed by Bruno Alfano
	https://os.mbed.com/users/brunoalfano/code/MPU6000_spi/
	@author Mike Angus
*/

#ifndef IMU_HPP
#define IMU_HPP

//Includes
#include <wiringPiSPI.h>	//for SPI bus control
#include <cstdint>	//for int types
#include <cerrno>	//for errno used by wiringpi
#include <cstdio>	//for printf
#include <unistd.h>	//for usleep
#include <iostream>	//for prompts during test

//Default settings
#define SPI_FREQ_HZ 100000
#define IMU_CS_NUM 0

// MPU6000 registers (Bruno Alfano)
#define MPUREG_XG_OFFS_TC 0x00
#define MPUREG_YG_OFFS_TC 0x01
#define MPUREG_ZG_OFFS_TC 0x02
#define MPUREG_X_FINE_GAIN 0x03
#define MPUREG_Y_FINE_GAIN 0x04
#define MPUREG_Z_FINE_GAIN 0x05
#define MPUREG_XA_OFFS_H 0x06
#define MPUREG_XA_OFFS_L 0x07
#define MPUREG_YA_OFFS_H 0x08
#define MPUREG_YA_OFFS_L 0x09
#define MPUREG_ZA_OFFS_H 0x0A
#define MPUREG_ZA_OFFS_L 0x0B
#define MPUREG_PRODUCT_ID 0x0C
#define MPUREG_SELF_TEST_X 0x0D
#define MPUREG_SELF_TEST_Y 0x0E
#define MPUREG_SELF_TEST_Z 0x0F
#define MPUREG_SELF_TEST_A 0x10
#define MPUREG_XG_OFFS_USRH 0x13
#define MPUREG_XG_OFFS_USRL 0x14
#define MPUREG_YG_OFFS_USRH 0x15
#define MPUREG_YG_OFFS_USRL 0x16
#define MPUREG_ZG_OFFS_USRH 0x17
#define MPUREG_ZG_OFFS_USRL 0x18
#define MPUREG_SMPLRT_DIV 0x19
#define MPUREG_CONFIG 0x1A
#define MPUREG_GYRO_CONFIG 0x1B
#define MPUREG_ACCEL_CONFIG 0x1C
#define MPUREG_INT_PIN_CFG 0x37
#define MPUREG_INT_ENABLE 0x38
#define MPUREG_ACCEL_XOUT_H 0x3B
#define MPUREG_ACCEL_XOUT_L 0x3C
#define MPUREG_ACCEL_YOUT_H 0x3D
#define MPUREG_ACCEL_YOUT_L 0x3E
#define MPUREG_ACCEL_ZOUT_H 0x3F
#define MPUREG_ACCEL_ZOUT_L 0x40
#define MPUREG_TEMP_OUT_H 0x41
#define MPUREG_TEMP_OUT_L 0x42
#define MPUREG_GYRO_XOUT_H 0x43
#define MPUREG_GYRO_XOUT_L 0x44
#define MPUREG_GYRO_YOUT_H 0x45
#define MPUREG_GYRO_YOUT_L 0x46
#define MPUREG_GYRO_ZOUT_H 0x47
#define MPUREG_GYRO_ZOUT_L 0x48
#define MPUREG_SIGNAL_PATH_RESET 0x68
#define MPUREG_USER_CTRL 0x6A
#define MPUREG_PWR_MGMT_1 0x6B
#define MPUREG_PWR_MGMT_2 0x6C
#define MPUREG_BANK_SEL 0x6D
#define MPUREG_MEM_START_ADDR 0x6E
#define MPUREG_MEM_R_W 0x6F
#define MPUREG_DMP_CFG_1 0x70
#define MPUREG_DMP_CFG_2 0x71
#define MPUREG_FIFO_COUNTH 0x72
#define MPUREG_FIFO_COUNTL 0x73
#define MPUREG_FIFO_R_W 0x74
#define MPUREG_WHOAMI 0x75
 
// Configuration bits MPU6000 (Bruno Alfano)
#define BIT_SLEEP 0x40
#define BIT_H_RESET 0x80
#define BITS_CLKSEL 0x07
#define MPU_CLK_SEL_PLLGYROX 0x01
#define MPU_CLK_SEL_PLLGYROZ 0x03
#define MPU_EXT_SYNC_GYROX 0x02
#define BITS_FS_250DPS              0x00
#define BITS_FS_500DPS              0x08
#define BITS_FS_1000DPS             0x10
#define BITS_FS_2000DPS             0x18
#define BITS_FS_2G                  0x00
#define BITS_FS_4G                  0x08
#define BITS_FS_8G                  0x10
#define BITS_FS_16G                 0x18
#define BITS_FS_MASK                0x18
#define BITS_DLPF_CFG_256HZ			0x00
#define BITS_DLPF_CFG_188HZ         0x01
#define BITS_DLPF_CFG_98HZ          0x02
#define BITS_DLPF_CFG_42HZ          0x03
#define BITS_DLPF_CFG_20HZ          0x04
#define BITS_DLPF_CFG_10HZ          0x05
#define BITS_DLPF_CFG_5HZ           0x06
#define BITS_DLPF_CFG_2100HZ_NOLPF  0x07
#define BITS_DLPF_CFG_MASK          0x07
#define BIT_INT_ANYRD_2CLEAR        0x10
#define BIT_RAW_RDY_EN              0x01
#define BIT_I2C_IF_DIS              0x10
 
#define READ_FLAG   0x80


/**
	IMU class provides methods for operating the MPU6000 inertial measurement unit.
	Datasheet and register map available at: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
*/
class IMU {

	public:
		/**
			@brief IMU constructor.
			Sets up an SPI device with speed SPI_FREQ_HZ, using chip select IMU_CS_NUM.
			Chip select may be 0 or 1 for Pi pins 24 and 26 respectively.
			We are using pin 24 for chip select, so IMU_CS_NUM will be 0.
		*/
		IMU();

		/**
			@brief Chip initialisation routine.
			Performs a software reset of the chip and then sets up ready to use.
			Sample rate and accel/gyro/filtering settings are initialised to chosen defaults here
			Note that there is 200ms of sleep time in this function.
		*/
		void init();

		/**
			@brief Set the measurement scale of the accelerometer.
			Higher values trade reduced numerical precision for increased range.
			@param option One of four predefined sets of bits:
			BITS_FS_2G = max g-force value +/- 2g
			BITS_FS_4G = max g-force value +/- 4g
			BITS_FS_8G = max g-force value +/- 8g
			BITS_FS_16G = max g-force value +/- 16g
		*/
		void setAccelerometerScale(uint8_t option);

		/**
			@brief Set the measurement scale of the gyroscope.
			Higher values trade reduced numerical precision for increased range.
			@param option One of four predefined sets of bits:
			BITS_FS_250DPS = max rotation rate value +/- 250 degrees/second
			BITS_FS_500DPS = max rotation rate value +/- 500 degrees/second
			BITS_FS_1000DPS = max rotation rate value +/- 1000 degrees/second
			BITS_FS_2000DPS = max rotation rate value +/- 2000 degrees/second
		*/
		void setGyroScale(uint8_t option);

		/**
			@brief Set the level of digital low-pass filtering (DLPF) applied to all measurements
			Defined by bandwidth in Hz, which corresponds to a particular delay in the time domain.
			Lower bandwidths mean better noise filtering, but higher delays.
			Choose the lowest bandwidth that is sufficient to capture the desired dynamics.
			Details of exact bandwidths and delays are on p13 of MPU6000 register map.
			@param option One of seven predefined sets of bits:
			BITS_DLPF_CFG_256HZ = filter bandwidth of 256Hz
			BITS_DLPF_CFG_188HZ = filter bandwidth of 188Hz
			BITS_DLPF_CFG_98HZ = filter bandwidth of 98Hz
			BITS_DLPF_CFG_42HZ = filter bandwidth of 42Hz
			BITS_DLPF_CFG_20HZ = filter bandwidth of 20Hz
			BITS_DLPF_CFG_10HZ = filter bandwidth of 10Hz
			BITS_DLPF_CFG_5HZ = filter bandwidth of 5Hz
		*/
		void setFilterBandwidth(uint8_t option);

		/**
			@brief Read the accelerometer in all three axes.
			Output is three float values in g (g-force, not grams)
			@param resultArray 3-element array to store the output.
			Order in the array is X, Y, Z in locations [0], [1], [2]
		*/
		void readAccel(float* resultArray);
		/**
			@brief Read the gyroscope in all three axes.
			Output is three float values in degrees per second
			@param resultArray 3-element array to store the output.
			Order in the array is X, Y, Z in locations [0], [1], [2]
		*/
		void readGyro(float* resultArray);
		/**
			@brief Read the temperature measured by the onboard sensor.
			Chip itself does not appear to heat up significantly, so this can be a proxy for board temperature.
			Confirmed with thermal camera that readings are about right.
			@return The temperature in degrees Celsius
		*/
		float readTemperature();

		/**
			@brief Perform self test routine
			@param testLength The number of samples to test the accel and gyro for
			@param samplePeriodUs The time between samples in microseconds
		*/
		void test(int testLength, int samplePeriodUs);

	private:
		//These factors are used to divide the values in the measurement registers
		//based on the chosen measurement range, to produce the actual readings.
		int accelerometerScaleFactor;
		float gyroScaleFactor;

		/**
			@brief Write a single byte to the target register over the SPI bus
			@param address Target register address
			@param data The data to be written to the register
		*/
		void write8To(uint8_t address, uint8_t data);
		/**
			@brief Read a single byte from the target register over the SPI bus
			@param address Target register address
			@return The contents of the register
		*/
		uint8_t read8From(uint8_t address);
		/**
			@brief Read two bytes from the target register over the SPI bus
			@param address Target register address
			This is taken to be the address of the high byte, with the low byte at the subsequent address
			@return The concatenated 16-bit value derived from the two bytes read
		*/
		uint16_t read16From(uint8_t address);

};

#endif