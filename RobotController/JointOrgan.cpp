/**
	@file JointOrgan.cpp
	@brief Implementation of JointOrgan methods (based on Mike's MotorOrgan)
	@author Matt
*/

#include "JointOrgan.hpp"

//Constructor
JointOrgan::JointOrgan(uint8_t address) : I2CDevice(address){
	//Just uses the I2CDevice constructor
}

//new Target is an 8-bit number (0 to 255) representing angles from 0degrees to 180degrees
void MotorOrgan::setTargetAngle(uint8_t newTarget) {
	//Set new target position
	targetPosition = newTarget;

	// ensure servo is active (good to do this every time, in case of brownout and arduino resets?)
	setServoOn()

	//Send to device (write8To inherited from I2CDevice)
	write8To(TARGET_POSTIION_REGISTER, targetPosition);
}

//Turn off servo so it should move freely
void JointOrgan::setServoOff() {
	//Send to device (write8To inherited from I2CDevice)
	write8To(JOINT_MODE_REGISTER, SERVO_OFF);
}

//Turn the servo on so it tries to acheive the target position
void JointOrgan::setServoOn() {
	//Send to device (write8To inherited from I2CDevice)
	write8To(JOINT_MODE_REGISTER, SERVO_ON);
}

uint8_t jointOrgan::readMeasuredAngle() {
	//Read and return the current angle, as measured by the potentiometer
	return read8From(MEASURED_POSITION_REGISTER);
}


