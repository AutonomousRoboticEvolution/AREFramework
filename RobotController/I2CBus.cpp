/**
	@file I2CBus.cpp
	@brief Implementation of I2CBus methods.
	@author Mike Angus
*/

#include "I2CBus.hpp"

//Constructor
I2CBus::I2CBus(char* busFilePath) {
	this->filePath = busFilePath;
}

//Open access to the bus
bool I2CBus::openBus(void) {
	if ((this->handle = open(filePath, O_RDWR)) < 0) {
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus\n");
		return 0;
	} else {
		return 1;
	}
}

//Close the bus access file
void I2CBus::closeBus(void) {
	close(this->handle);
}