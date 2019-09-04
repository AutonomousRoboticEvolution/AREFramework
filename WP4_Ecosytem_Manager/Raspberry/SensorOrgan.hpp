/**
	@file SensorOrgan.hpp
	@brief Class declaration for SensorOrgan and I2CSwitch.
	@author Mike Angus
*/

#ifndef SENSORORGAN_HPP
#define SENSORORGAN_HPP

#include "I2CDevice.hpp"
#include "Sensors.hpp"

#define VCNL4040_ADDRESS 0x60
#define TOF_ADDRESS 0x29
#define COLORSENSOR_ADDRESS 0x10
#define ADC_MUDULE 0x4D

/**
	Controller class for the TCA9543A I2C switch.

	The address of the device is set using the DIP switches on the PCB. There are
	two address pins A0 and A1, which can each be in one of two states:
	Vcc or GND, enabling a total of 4 unique addresses. The DIP switches
	are wired as follows:

	\code
	Vcc GND Vcc GND
	 |   |   |   |
	 /   /   /   /
	 |   |   |   |
	 1   2   3   4
	  \ /     \ /
	   A1      A0
	\endcode

	This produces addresses as follows:
	A1, A0 = GND, GND => 1110000 = 0x70
	A1, A0 = GND, VCC => 1110001 = 0x71
	A1, A0 = VCC, GND => 1110010 = 0x72
	A1, A0 = VCC, VCC => 1110011 - 0x73

*/
class I2CSwitch  : protected I2CDevice {
	public :
		//Public member functions
		/**
			@brief I2CSwitch constructor.
			This simply calls the superclass constructor I2CDevice()
			@param address The I2C device address, using 7-bit version
			 (see I2CDevice class description for explanation)
		*/
		I2CSwitch(uint8_t address);
		/**
			@brief Open the I2C bus to the devices on the sensor board. 
			This must be done before any attempted access to the sensors.
		*/
		void enableChannel();
		/**
			@brief Close the I2C bus to the devices on the sensor board.
			This must be done after any access to sensors, to prevent
			address conflicts with other sensor organ instances.
		*/
		void disableChannel();
};

/**
	Class for the multi-sensor organ.
	The purpose of this is to encapsulate the functions of
	the various sensors on the board, and control access to them
	via the I2C switch on the interface board, to allow multiple
	sensor boards to be used.
*/
class SensorOrgan {
	private:
		I2CSwitch i2cSwitch;	///<The I2C switch specific to this sensor organ

	public :
		uint8_t address;	///<I2C address of the switch
		ProximitySensor proximity; ///<Proximity sensor on the board
		AmbientLightSensor ambientLight; ///<Ambient light sensor on the board

		ADCmodule adcModule;

		ColorSensor color;
	
		TOF tof;

		/**
			@brief SensorOrgan constructor. Sets up i2cSwitch and addresses for sensors.
			@param switchAddress The I2C device address of the I2C switch
		*/
		SensorOrgan(uint8_t switchAddress);
		/**
			@brief Initialise the proximity sensor before use.
		*/
		void initProximity();
		/**
			@brief Read the proximity sensor's current value.
			@return Raw reading from the sensor
		*/
		uint16_t readProximity();

		/**
		 * @brief Read the proximity sensor's current value.
		 * @return Reading from the sensor converted in meters
		 */
        float calibratedProximityReading();
		/**
			@brief Initialise the proximity sensor before use.
		*/
		void initAmbientLight();
		/**
			@brief Read the proximity sensor's current value.
			@return Raw reading from the sensor
		*/
		uint16_t readAmbientLight();
		/**
			@brief Read the white value from the ambient light sensor
			@return Raw reading from the sensor
		*/
		uint16_t readWhite();

		/**
			@brief Initialise the ADC module before use.
		*/
		void ADCinit();
		/**
			@brief Read the proximity ADC's current value.
			@return Raw reading from the ADC
		*/
		uint16_t ADCdata();

		/**
			@brief Initialise the color sensor before use.
		*/
		void ColorSensorinit();
		/**
			@brief Read the color sensor's current value.
			@return Raw reading from the color sensor
		*/
		uint16_t read_color_channel(uint8_t ch);
	
		/**
			@brief Initialise the color sensor before use.
		*/
		// Opens a file system handle to the I2C device
		// sets the device continous capture mode
		void initTOF(int iChan, int bLongRange);
		
		/**
			@brief Read the current distance in mm
			@return distance from the TOF
		*/
		int readDistanceTOF(void);


};

#endif
