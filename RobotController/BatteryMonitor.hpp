/**
	@file BatteryMonitor.hpp
	@brief Header and documentation for BatteryMonitor.cpp, for the DS2438Z+ battery monitor chip, via I2C converter
	@author Matt Hale
*/

#ifndef BATTERY_MONITOR_HPP
#define BATTERY_MONITOR_HPP

#include "I2CDevice.hpp"

#define BATTERY_MONITOR_I2C_ADDRESS 0x18

/**
 * these are the commands the i2c-to-1wire converter understands:
 */
#define COMMAND_DEVICE_RESET 0xF0
#define COMMAND_SET_READ_POINTER 0xE1
#define COMMAND_WRITE_DEVICE_CONFIGURATION 0xD2
#define COMMAND_ADJUST_1_WIRE_PORT 0xC3
#define COMMAND_1_WIRE_RESET 0xB4
#define COMMAND_1_WIRE_SINGLE_BIT 0x87
#define COMMAND_1_WIRE_WRITE_BYTE 0xA5
#define COMMAND_1_WIRE_READ_BYTE 0x96

/**
 these are used after BATTERY_MONITOR_COMMAND_SET_READ_POINTER to define where to move the read pointer:
 */
#define POINTER_CODE_DEVICE_CONFIGURATION 0xC3
#define POINTER_CODE_STATUS 0xF0
#define POINTER_CODE_READ_DATA 0xE1
#define POINTER_CODE_PORT_CONFIGURATION 0xB4


#define CRC_POLYNOMIAL 0b100110001 // defined by the maxim 1Wire CRC standard
#define CRC_INITIAL_REMAINDER 0 // defined by the maxim 1Wire CRC standard
#define CRC_CHECK_MAX_RETRIES 10 // if the CRC check fails, we will try again. after this many retries, throw an error



/**
     bit7         bit6              bit5             bit4          bit3          bit2           bit1            bit0
 !1-WireSpeed !StrongPullup   !1-WirePowerDown  !ActivePullup  1-WireSpeed  StrongPullup   1-WirePowerDown  ActivePullup
      1            1                  1               0             0             0               0              1
 */
#define DS2484_DEFAULT_CONFIGURATION_DATA 0xE1


class BatteryMonitor : protected I2CDevice {

	public :
		BatteryMonitor();
	    void init();
		void direct1WireTest();
		int measureTemperature();
		int measureVoltage();
        void printAllPages();


    private:
        /**
         * @brief use this to send a single byte to the downstream 1Wire device, i.e. the battery monitor itself
         * @param data the the to send
         */
        void write1WireByte(int data);

        /**
         * @brief use this to read a single byte from the downstream 1Wire device, i.e. the battery monitor itself
         * @return the byte read
         */
        int read1WireByte();
        void setReadPointer(int);
        void waitFor1WireToFinish();
        void reset1Wire();
        void skipROM();

        int pageDataFromDS2438 [8]; // for temporarily holding the raw 8-bytes of page data
        void updateReadPageData(int pageNumber);
        void waitForDS2438ToStopBeingBusy();

        /**
         * @brief confirms a given message against its CRC to check for transmission errors
         * @return zero if the test is passed, a non-zero value otherwise
         * @param message: the set of bytes received from the battery monitor
         * @param nBytes: how many bytes in message
         * @receivedCRC the CRC value from the battery monitor (i.e. the 9th byte in the transaction) against which to check the message
         */
        int doCRCCheck(int message[], int nBytes, int receivedCRC);

};

#endif