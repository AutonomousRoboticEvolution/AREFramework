/**
	@file I2CDevice.hpp
	@brief Class declaration for I2CDevice
	@author Mike Angus
*/

#ifndef I2CDEVICE_HPP
#define I2CDEVICE_HPP

#include <stdio.h>
#include <stdint.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/**
	Generic I2C device class, to be extended as required by various I2C organs.
	Functions are based on the  
	<a href="http://wiringpi.com/reference/i2c-library/">I2C components of the wiringPi library</a>

    Note that the most likely cause of a new device not working is an addressing
	issue. Datasheets vary as to how they specify the device address; sometimes it
	will be in 8-bit format. Since bit [0] is always the read/write selection
	bit, the address itself is actually bits [7:1], and these comprise the 7-bit
	format used by this class. For example: an 8-bit address given in the
	datasheet as 0xD0 (011010000) must be shifted right by 1 bit to give 0x68
	(001101000) in order to produce a valid address which can be used by this
	class. 
*/

class I2CDevice {
	public :
		uint8_t devAddress; ///< The I2C address of the device

		/**
			@brief I2CDevice constructor. Sets devAddress and retrieves devHandle, which is 
			used for all subsequent accesses to the device 
			@param address The I2C device address, using 7-bit version
			 (see class description for explanation)
		*/
		I2CDevice(uint8_t address);

		/**
			@brief Write 8-bit value directly to device (no subaddress), for cases when the
			internal register doesn't have it's own address.
			@param data The byte to be written
		*/
		void write8(uint8_t data);

		/**
			@brief Write 8-bit value to specified register of device.
			@param subAddress The address of the target internal register (not device address)
			@param data The byte to be written
		*/
		void write8To(uint8_t subAddress, uint8_t data);

		/**
			@brief Write 16-bit value to specified register of device.
			@param subAddress The address of the target internal register (not device address)
			@param data The 16-bit word to be written
		*/
		void write16To(uint8_t subAddress, uint16_t data);

		/**
			@brief Write 16-bit value in little-endian format to specified register of device.
			@param subAddress The address of the target internal register (not device address)
			@param data The 16-bit word to be written
		*/
		void writeLittleEndian16To(uint8_t subAddress, uint16_t data);

		/**
			@brief Read 8-bit value directly from device (no subaddress), for cases when the
			internal register doesn't have it's own address.
			@return The data read from the device
		*/
		int read8();

		/**
			@brief Read 8-bit value from specified register of device.
			@param subAddress The address of the target internal register (not device address)
			@return The data read from the device
		*/
		int read8From(uint8_t subAddress);

		/**
			@brief Read 16-bit value from specified register of device.
			@param subAddress The address of the target internal register (not device address)
			@return The data read from the device
		*/
		int read16From(uint8_t subAddress);

		/**
			@brief Read 16-bit value in little-endian format from specified register of device.
			@param subAddress The address of the target internal register (not device address)
			@return The data read from the device
		*/
		int readLittleEndian16From(uint8_t subAddress);

	protected :
		int devHandle;	///< The handle to the bus access file for this device
};

#endif