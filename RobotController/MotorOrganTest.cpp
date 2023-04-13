/**
	@file MotorOrganTest.cpp
	@brief Test code for the MotorOrgan class.
	Allows control of motor and shows how the bus should be set up.
	@author Mike Angus
*/

#include "I2CBus.hpp"
#include "MotorOrgan.hpp"

#define MOTOR_ADDRESS 0x67

int main(void) {

	//Initialise the bus
	I2CBus i2cBus1((char*)"/dev/i2c-1");
	i2cBus1.openBus();

	//Create a motor organ
	MotorOrgan motor1(MOTOR_ADDRESS);

	//TEST Set motor speed from console
	int8_t spd = 0;
	char c;
	printf("MOTOR TEST\np increases\no decreases\nk brakes\nc clears fault reg\nx exits\n");
	do {
		printf("Speed: %d\n", spd);
		c = getchar();
		switch (c) {
			case 'p' :
				spd++;
				motor1.setSpeed(spd);
				break;

			case 'o' :
				spd--;
				motor1.setSpeed(spd);
				break;

			case 'k' :
				spd = 0;
				motor1.brake();
				break;
			case 'c' :
				motor1.clearFaultReg();
				printf("FAULTREG: %02X\n", motor1.readFaultReg());
				break;
		}
		//printf("FAULTREG: %02X\n", motor1.readFaultReg());
	} while (c != 'x');

	return 1;
}