/**
	@file I2CBus.hpp
	@brief Class declaration for I2CBus
	@author Mike Angus
*/

#ifndef I2CBUS_HPP
#define I2CBUS_HPP

#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <stdio.h>
#include <stdint.h>

/**
	Raspberry Pi I2C bus access class.
	Provides the means to initialise access to the I2C bus on the Pi,
	generating a handle which can be used by ioctl operations in I2CDevice.

	To detect devices on the bus from the command line, invoke sudo i2cdevice -y 1,
	where 1 refers to which I2C bus is to be scanned.
*/
class I2CBus {
	public :
		int handle; ///<Handle which can be used to access the bus

		/**
			@brief I2CBus Constructor.
			@param busFilePath a string with a path to the i2c bus
				- "/dev/i2c-0" for bus 0
				- "/dev/i2c-1" for bus 1
		*/
		I2CBus(char* busFilePath);
		/**
			@brief Initialises access to the i2c bus by opening the file and providing a handle for access
				This function must be called before the I2CBus object can be used
			@return boolean; true if operation successful, false if not
		*/
		bool openBus();
		/**
			@brief Closes the bus access file.
		*/
		void closeBus();

	private :
		char* filePath; ///<String containing the path to the I2C device file
};

#endif