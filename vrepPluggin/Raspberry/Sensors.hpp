/**
	@file Sensors.hpp
	@brief Class declaration for individual sensors on the multi-sensor board.
	@author Mike Angus
*/

#ifndef SENSORS_HPP
#define SENSORS_HPP

#include "I2CDevice.hpp"

//************TOF***************
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
//************TOF***************

//Register subaddresses for VCNL4040 proximity Sensor
//These are all 16-bit registers
#define PS_CONF1_CONF2_REG 0x03
#define PS_CONF3_MS_REG 0x04
#define PS_DATA_REG 0x08

//Register subaddresses for VCNL4040 Ambient Light sensor
#define ALS_CONF_REG 0x00
#define ALS_DATA_REG 0x09
#define WHITE_DATA_REG 0x0A

/**
	Class for the proximity sensor component of the 
	<a href="https://www.vishay.com/docs/84274/vcnl4040.pdf>VCNL4040"
*/
class ProximitySensor  : protected I2CDevice {
	public :
		/**
			@brief ProximitySensor constructor.
			This simply calls the superclass constructor I2CDevice()
			@param address The I2C device address, using 7-bit version
			 (see I2CDevice class description for explanation)
		*/
		ProximitySensor(uint8_t address);
		/**
			@brief Initialise the proximity sensor
		*/
		void init();
		/**
			@brief Read the proximity sensor
			@return Raw reading from the sensor
		*/
		uint16_t read();
};

/**
	Class for the ambient light sensor component of the 
	<a href="https://www.vishay.com/docs/84274/vcnl4040.pdf>VCNL4040"
*/
class AmbientLightSensor : protected I2CDevice {
	public :
		/**
			@brief AmbientLightSensor constructor.
			This simply calls the superclass constructor I2CDevice()
			@param address The I2C device address, using 7-bit version
			 (see I2CDevice class description for explanation)
		*/
		AmbientLightSensor(uint8_t address);
		/**
			@brief Initialise the ambient light sensor
		*/
		void init();
		/**
			@brief Read the ambient light sensor
			@return Raw reading from the sensor
		*/
		uint16_t readAmbient();
		/**
			@brief Read the white value from the ambient light sensor
			@return Raw reading from the sensor
		*/
		uint16_t readWhite();
};


/*
	Class fot Low-Power 12-Bit A/D Converter MCP3221
*/
class ADCmodule : protected I2CDevice
{
private:
	/* data */
public:
	/**
		@brief ADCmodule constructor.
		This simply calls the superclass constructor I2CDevice()
		@param address The I2C device address, using 7-bit version
			(see I2CDevice class description for explanation)
	*/
	ADCmodule(uint8_t address);

	/**
		@brief Initialise the ADC module
	*/
	void init();

	/**
		@brief Read the ADC module
		@return Raw reading from the ADC module
	*/
	uint16_t ADCread();

};

/*
		color sensor
*/
class ColorSensor : protected I2CDevice
{
private:
	/* data */
public:
	/**
		@brief Color Sensor constructor.
		This simply calls the superclass constructor I2CDevice()
		@param address The I2C device address, using 7-bit version
			(see I2CDevice class description for explanation)
	*/
	ColorSensor(uint8_t address);

	/**
		@brief Initialise the color sensor
	*/
	void init();

	/**
		@brief Read the color sensor
		@return Raw reading from the color sensor (4channel of colors RGBW and represented by 0 1 2 3)
	*/
	uint16_t readcolor(uint8_t ch);
	
};

class TOF : protected I2CDevice {
	public:
		
		TOF(uint8_t address);
			
		int getModel(int *model, int *revision);
		
		//
		// Read the current distance in mm
		//
		int readDistance(void);

		//
		// Opens a file system handle to the I2C device
		// sets the device continous capture mode
		//
		int init(int iChan, int bLongRange);
			
	private:
		uint8_t iAddr;
	    int file_i2c;
		unsigned char stop_variable;
		uint32_t measurement_timing_budget_us;
		typedef enum vcselperiodtype { VcselPeriodPreRange, VcselPeriodFinalRange } vcselPeriodType;
		typedef struct tagSequenceStepTimeouts
		{
		  uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;

		  uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
		  uint32_t msrc_dss_tcc_us,    pre_range_us,    final_range_us;
		} SequenceStepTimeouts;
	
		unsigned char readReg(unsigned char ucAddr);
		unsigned short readReg16(unsigned char ucAddr);
		void writeReg16(unsigned char ucAddr, unsigned short usValue);
		void writeReg(unsigned char ucAddr, unsigned char ucValue);
		void writeRegList(unsigned char *ucList);
		int performSingleRefCalibration(uint8_t vhv_init_byte);
		int setMeasurementTimingBudget(uint32_t budget_us);
	    int setVcselPulsePeriod(vcselPeriodType type, uint8_t period_pclks);
		void readMulti(unsigned char ucAddr, unsigned char *pBuf, int iCount);
		void writeMulti(unsigned char ucAddr, unsigned char *pBuf, int iCount);
		int getSpadInfo(unsigned char *pCount, unsigned char *pTypeIsAperture);
		uint16_t decodeTimeout(uint16_t reg_val);
		uint32_t timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks);
		uint32_t timeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks);
		uint16_t encodeTimeout(uint16_t timeout_mclks);
		void getSequenceStepTimeouts(uint8_t enables, SequenceStepTimeouts * timeouts);
		uint32_t getMeasurementTimingBudget(void);
		int initSensor(int bLongRangeMode);
		uint16_t readRangeContinuousMillimeters(void);
};


#endif