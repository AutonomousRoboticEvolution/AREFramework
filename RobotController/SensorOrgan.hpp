/**
        @file SensorOrgan.hpp
        @brief Class declaration for SensorOrgan and I2CSwitch.
        @author Mike Angus
*/

#ifndef SENSORORGAN_HPP
#define SENSORORGAN_HPP

#include "Organ.hpp"
//#include "VL53L0X.h" // not needed, as the Arduino does the settup of the VL53L0X
#include <iostream> // for cout debugging
#include <wiringPi.h> // for millis()
#include <bitset>

//the registers, these must match those defined in the Arduino firmware
#define REQUEST_TIME_OF_FLIGHT_REGISTER 0x01
#define REQUEST_INFRARED_REGISTER 0x02
#define FLASH_INDICATOR_LED_REGISTER 0x03
#define SET_TIME_OF_FLIGHT_ADDRESS_REGISTER 0x04 // set the i2c address of the VL53L0X sensor
#define GET_TIME_OF_FLIGHT_ADDRESS_REGISTER 0x05 // return what the arduino thinks is the i2c address of the VL53L0X sensor
#define REQUEST_INFRARED_RAW_VALUE_REGISTER 0x06 // return the raw IR value, without the filtering

#define TIMEOUT_WHEN_RANGEFINDING_MILLISECONDS 5
#define MAXIMUM_RANGE_VALUE 2000
#define VALUE_TO_RETURN_ON_TIMEOUT MAXIMUM_RANGE_VALUE
#define VL53L0X_RESULT_RANGE_STATUS 0x14
#define VL53L0X_RESULT_INTERRUPT_STATUS 0x13
#define VL53L0X_SYSTEM_INTERRUPT_CLEAR 0x0B
#define VL53L0X_RESULT_INTERRUPT_STATUS 0x13

/**
 * @brief The VL53L0X class
 * the time of flight sensor (VL53L0X) is a separate I2Cdevice from the rest of the sensor organ
 * Thus, the pi commuincates directly with it to retreive TOF values, not via the Arduino
 * Note, however, the Arduino is responsible for the initial setup of the VL53L0X, including changing it's i2c address from the default.
 */
class VL53L0X: protected I2CDevice {
	public:
		VL53L0X(uint8_t address);
		// make some of the underlying private methods public so the SensorOrgan class can call them:
    	using I2CDevice::read8From;
    	using I2CDevice::read8;
		using I2CDevice::write8To;
};

/**
        Class for the sensor organ
*/
class SensorOrgan  : public Organ {
    public :

        //Public variables
        boardSelection daughterBoardToEnable = BOTH; // to store which daughterboard this organ is attached to, so must be enabled before use

        SensorOrgan(uint8_t address);
        uint16_t readInfrared();
        uint16_t readInfraredRaw();
        uint16_t readTimeOfFlight();
        uint16_t getTimeOfFlightI2CAddress();
        void flashIndicatorLED(uint8_t numberOfFlashes);
        void setTimOfFlightI2CAddress(uint8_t newAddress);

        OrganType organType = SENSOR;

	private:
        VL53L0X* timeOfFlight;
};


#endif
