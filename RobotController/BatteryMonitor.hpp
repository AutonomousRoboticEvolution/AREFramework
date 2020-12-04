/**
	@file BatteryMonitor.hpp
	@brief Header and documentation for BatteryMonitor.cpp, for the DS2438Z+ battery monitor chip, via I2C converter
	@author Matt Hale
*/

#ifndef BATTERY_MONITOR_HPP
#define BATTERY_MONITOR_HPP

#include "I2CDevice.hpp"

/**
 * The factor for converting from the register value to an actual current depends upon the value of sense resistor used.
 * The current value (in Amps) is given in the datashet as I = (register value)/(4096*Rsense) for I in Amps, Rsense in Ohms
 * To make the numbers reasonable to read as 16-bit signed integers, here we use mA for the current value,
 * found by the equation I_milli_Amps = (register value) * BATTERY_MONITOR_CURRENT_CONVERSION_FACTOR
 * where BATTERY_MONITOR_CURRENT_CONVERSION_FACTOR = (1000)/(4096*R_SENSE_IN_OHMS)
 * e.g. for R_SENSE=0.05 Ohms, BATTERY_MONITOR_CURRENT_CONVERSION_FACTOR=4.88, so the LSB of the current register corresponds to 4.88mA.
 */
#define BATTER_MONITOR_R_SENSE 0.05
#define BATTERY_MONITOR_CURRENT_CONVERSION_FACTOR 1000/(4096*BATTER_MONITOR_R_SENSE)
/**
 * The equivilent for the remaining battery capacity measurement ("ICA" in datasheet) is:
 * Remaining Capacity = ICA / (2048 * R_SENSE_IN_OHMS )
 * So for a result in mAh, we can calculate another factor as follows
 */
#define BATTERY_MONITOR_CURRENT_ACCUMULATOR_CONVERSION_FACTOR 1000/(2096*BATTER_MONITOR_R_SENSE)


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
#define BATTERY_MONITOR_PAGE_ICA 1
#define BATTERY_MONITOR_BYTE_ICA 4


/**
 *  This is the configuration byte for the i2c to 1wire converter (DS2484).
 *  When setting, the MSB 4 bits need to be the one's compliment of the LSB 4, which actually contain the settings.
 *  Note when reading it back, the MSB 4 bits will all appear zeros.
     bit7         bit6              bit5             bit4          bit3          bit2           bit1            bit0
 !1-WireSpeed !StrongPullup   !1-WirePowerDown  !ActivePullup  1-WireSpeed  StrongPullup   1-WirePowerDown  ActivePullup
      1            1                  1               0             0             0               0              1
 */
#define DS2484_DEFAULT_CONFIGURATION_DATA 0xE1


class BatteryMonitor : protected I2CDevice {

	public :
		BatteryMonitor();

	    /**
	     * set up the i2c to 1Wire converter and the battery monitor chip. Called inside the class constructor.
	     */
	    void init();

	    /**
	     * @brief just for testing, displays voltage, current and ICA in a loop
	     */
        void testingDisplayBatteryLevels();

        /**
         * @brief measure the temperature of the battery monitor chip (on the motherboard). Smallest increment will be 0.03125 degrees.
         * @return a float value of the temperature in degrees C.
         */
        float measureTemperature();

        /**
         * @brief read the current current value from the battery monitor
         * @return the current in milliAmps. Note negative values indicate current is being drawn from the battery, positive values indicate the battery is being charged.
         */
        int measureCurrent();

        /**
         * @brief read the "ICA" value from the battery monitor, which represents an estimate of charge remaining
         * @return the charge remaining in milliAmpHours
         */
        int measureBatteryChargeRemaining();

        /**
         * @brief set the "ICA" value from the battery monitor, which represents an estimate of charge remaining
         * For example, when replacing the battery this should be called to reset the charge remaining
         * Note this will be converted to a value understood by the chip and will lose some precision, so the read value may not be exactly the same
         */
        void setBatteryChargeRemaining(int chargeValueMilliAmpHours);

        /**
         * @brief measure the voltage of the 5V bus. LSB represents 10mV. Note the maximum voltage the chip can read is 10V
         * @return the data as returned by the DS2438, which is an integer voltage measured in multiples of 10mV. e.g. 0x010E = 270 is 2.7V, or 0x01F4 = 500 is 5V.
         */
		uint16_t measure5VBusVoltage();

		/**
         * @brief measure the voltage of the battery. LSB represents 10mV. Note the maximum voltage the chip can read is stated as 10V in the datasheet
         * @return the data as returned by the DS2438, which is an integer voltage measured in multiples of 10mV. e.g. 0x010E = 270 is 2.7V, or 0x01F4 = 500 is 5V.
         */
		uint16_t measureBatteryVoltage();

		/**
		 * for debugging, print all the pages of the battery monitor memory in binary
		 */
        void printAllPages();


    private:
        /**
         * @brief send a single byte to the downstream 1Wire device, i.e. the battery monitor itself
         * @param the byte to send
         */
        void write1WireByte(int data);

        /**
         * @brief read a single byte from the downstream 1Wire device, i.e. the battery monitor itself
         * @return the byte read
         */
        int read1WireByte();

        /**
         * @brief for reading from the i2c to 1Wire converter, first use this function to set which register you want to read from, then do read8().
         * @param pointerCode: the register from the DS2848 you want to read
         */
        void setI2CReadPointer(int pointerCode);

        /**
         * @brief keeps checking if the 1wire bus is busy until it is not.
         * Use to ensure the i2c to 1Wire converter has finished its conversation with the battery monitor before you do the next thing (e.g. reading the values)
         */
        void waitFor1WireToFinish();

        /**
         * @brief do the 1Wire reset-presence sequence
         */
        void reset1Wire();

        /**
         * @brief tell the DS2438 battery monitor chip not to do a ROM check
         * The ROM checking is a feature to allow more than one slave device on the 1wire bus. As we only have one device, we can just skip it every time. See the examples in the datasheet (page 22-24) for when this needs to happen.
         */
        void skipROM();

        int pageDataFromDS2438 [8]; // for temporarily holding the raw 8-bytes of page data
        void updateReadPageData(int pageNumber); // tell the battery monitor to copy its memory into scratchpad, then copy the scratchpad values into pageDataFromDS2438
        void updateReadPageDataFromScratchpad(int pageNumber); // copy the scratchpad values into pageDataFromDS2438

        /**
         * @brief check if the battery monitor (DS2438) is busy doing a temperature or voltage conversion, and wait until it is not.
         */
        void waitForDS2438ToStopBeingBusy();

        /**
         * @brief confirms a given message against its CRC to check for transmission errors
         * @return zero if the test is passed, a non-zero value otherwise
         * @param message: the set of bytes received from the battery monitor
         * @param nBytes: how many bytes in message (usually 8)
         * @receivedCRC the CRC value from the battery monitor (i.e. the 9th byte in the transaction) against which to check the message
         */
        int doCRCCheck(int message[], int nBytes, int receivedCRC);

        /**
         * @brief Upload a new status/config byte to the DS2438, format is defined on p15 of the datasheet.
         * @param statusConfigByte the new value to be uplaoded.
         */
        void uploadNewDS2438Config(uint8_t statusConfigByte);

        /**
        * @brief write new data to the battery monitor
         * Will re-download from the scratchpad and check for errors (caused by the 1Wire interface) before commanding it be copied into the actual memory.
        * @param page which page the new byte needs to go into
        * @param byteNumber which byte within that page is being written
         * @param newByteValue the value to put there
        */
        void uploadNewByteToDS2438(int pageNumber, int byteNumber, uint8_t newByteValue);

};

#endif