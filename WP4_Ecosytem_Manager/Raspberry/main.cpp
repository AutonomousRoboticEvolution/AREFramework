/**
	@file MotorOrganTest.cpp
	@brief Test code for the MotorOrgan class.
	Allows control of motor and shows how the bus should be set up.
	@author Mike Angus
*/
#include <stdio.h>
#include "SensorOrgan.hpp"
#include "MotorOrgan.hpp"

#define SENSOR1 0x72
#define SENSOR2 0x73
#define MOTOR1 0x66
#define MOTOR2 0x68

int main(void) {
	// Variables
	int8_t speed1 = 0;
	int8_t speed2 = 0;
	uint16_t reading1 = 0;
	uint16_t reading2 = 0;
	//Create a sensor and motor  organs
	SensorOrgan sensor1(SENSOR1);
	SensorOrgan sensor2(SENSOR2);
	MotorOrgan motor1(MOTOR1);
	MotorOrgan motor2(MOTOR2);
	// Initialize motors
	motor1.brake();
	motor2.brake();
	sleep(1); // Give chance to stop motor at start of code
	// Initialize ADC sensor
	//sensor1.ADCinit();
	//sensor2.ADCinit();
	sensor1.initProximity();
	sensor2.initProximity();
	// Infinite loop
	do {
		// Take readings from sensors
		reading1 = sensor1.readProximity();
		reading2 = sensor2.readProximity();
		printf("Sensor 1 reading: %d\n", reading1);
		printf("Sensor 2 reading: %d\n", reading2);
		speed1 = 35;  //35
		speed2 = 35;  //35
		// Bang-bang controller
		if(reading1 > 100)
			speed2 = 0;
		if(reading2 > 100)
			speed1 = 0;
		// Turn on motors
	  	motor1.setSpeed(speed1);
		motor2.setSpeed(speed2);
		//Loop timer
		//usleep(100000); //100ms
		usleep(1000000);
	} while (1);
	motor1.brake();
	motor2.brake();
	return 1;
}
