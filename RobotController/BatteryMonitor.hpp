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
 * these are the commands the battery monitor chip itself (DS2438) understands:
 */
#define BATTERY_MONITOR_CONVERT_TEMPERATURE 0x44
#define BATTERY_MONITOR_CONVERT_VOLTAGE 0xB4
#define BATTERY_MONITOR_COPY_MEMORY_TO_SCRATCHPAD 0xB8
#define BATTERY_MONITOR_COPY_SCRATCHPAD_TO_MEMORY 0x48
#define BATTERY_MONITOR_READ_SCRATCHPAD 0xBE
#define BATTERY_MONITOR_WRITE_SCRATCHPAD 0x4E

#define BATTERY_MONITOR_BITMASK_CONFIG_CURRENT_AD 0x01
#define BATTERY_MONITOR_BITMASK_CONFIG_CURRENT_ACCUMULATOR 0x02
#define BATTERY_MONITOR_BITMASK_CONFIG_CURRENT_ACCUMULATOR_SHADOW 0x04
#define BATTERY_MONITOR_BITMASK_CONFIG_VOLTAGE_AD 0x08
#define BATTERY_MONITOR_BITMASK_CONFIG_TEMPERATURE_BUSY 0x10
#define BATTERY_MONITOR_BITMASK_CONFIG_MEMORY_BUSY 0x20
#define BATTERY_MONITOR_BITMASK_CONFIG_AD_BUSY 0x40

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
#define BATTERY_MONITOR_UPLOAD_MAX_RETRIES 10 // when uploading values, they will be downloaded again and checked. after this many retries, throw an error



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

        /**
         * @brief measure the temperature of the battery monitor chip (on the motherboard). Smallest increment will be 0.03125 degrees.
         * @return a float value of the temperature in degrees C.
         */
        float measureTemperature();

        /**
         * @brief measure the voltage of the 5V bus. Each bit represents 10mV. Note the maximum voltage the chip can read is 10V
         * @return the data as returned by the DS2438, which is an integer voltage measured in multiples of 10mV. e.g. 0x010E = 270 is 2.7V, or 0x01F4 = 500 is 5V.
         */
		uint16_t measure5VBusVoltage();

		/**
         * @brief measure the voltage of the battery. Each bit represents 10mV. Note the maximum voltage the chip can read is 10V
         * @return the data as returned by the DS2438, which is an integer voltage measured in multiples of 10mV. e.g. 0x010E = 270 is 2.7V, or 0x01F4 = 500 is 5V.
         */
		uint16_t measureBatteryVoltage();

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
        void setI2CReadPointer(int);
        void waitFor1WireToFinish();
        void reset1Wire();
        void skipROM();

        int pageDataFromDS2438 [8]; // for temporarily holding the raw 8-bytes of page data
        void updateReadPageData(int pageNumber); // tell the battery monitor to copy its memory into scratchpad, then copy the scratchpad values into pageDataFromDS2438
        void updateReadPageDataFromScratchpad(int pageNumber); // copy the scratchpad values into pageDataFromDS2438
        void waitForDS2438ToStopBeingBusy();

        /**
         * @brief confirms a given message against its CRC to check for transmission errors
         * @return zero if the test is passed, a non-zero value otherwise
         * @param message: the set of bytes received from the battery monitor
         * @param nBytes: how many bytes in message
         * @receivedCRC the CRC value from the battery monitor (i.e. the 9th byte in the transaction) against which to check the message
         */
        int doCRCCheck(int message[], int nBytes, int receivedCRC);

        /**
         * @brief Upload a new status/config byte to the DS2438, format is defined on p15 of the datasheet.
         * Will re-download from the scratchpad and check for errors (caused by the 1Wire interface) before commanding it be copied into the actual memory.
         * @param statusConfigByte the new value to be uplaoded.
         */
        void uploadNewConfig(uint8_t statusConfigByte);

};

#endif