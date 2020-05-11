/**
	@file JointOrganTest.cpp
	@brief Test code for the JointOrgan class (based on Mike's MotorOrgan)
	@author Matt
*/

#include "I2CBus.hpp"
#include "JointOrgan.hpp"

#define JOINT1_ADDRESS 0x07

int main(void) {

	//Initialise the bus
	I2CBus i2cBus1((char*)"/dev/i2c-1");
	i2cBus1.openBus();

	//Create the first joint organ
	JointOrgan joint1(&i2cBus1, JOINT1_ADDRESS);

	//TEST Set joint angle from console
	uint8_t angle = 90;
	char c;
	printf("JOINT TEST\np increases angle\no decreases angle\nk turns off\nl keeps on but doens't move\nx exits\n");
	do {
		printf("Speed: %d\n", spd);
		c = getchar();
		switch (c) {
			case 'p' :
				angle+=20;
				joint1.setTargetAngle(angle);
				break;

			case 'o' :
				angle-=20;
				joint1.setTargetAngle(angle);
				break;

			case 'k' :
				joint1.setServoOff();
				break;

			case 'l' :
				joint1.setServoOn();
				break;
		}
		printf("Current target angle: %u\n", angle);
		printf("Current measured angle: %u\n", joint1.readMeasuredAngle());
	} while (c != 'x');

	return 1;
}
