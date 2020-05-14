/**
	@file JointOrgan.cpp
	@brief Implementation of JointOrgan methods (based on Mike's MotorOrgan)
	@author Matt
*/

#include "JointOrgan.hpp"


#define NUMBER_OF_RETRIES_BEFORE_GIVING_UP 10

//Constructor
JointOrgan::JointOrgan(uint8_t address) : I2CDevice(address){
	//Just uses the I2CDevice constructor
}

//new Target is an 8-bit number (0 to 255) representing angles from 0degrees to 180degrees

void JointOrgan::setTargetAngle(uint8_t newTarget) {
	//Send to device (write8To inherited from I2CDevice)
	write8To(TARGET_POSTIION_REGISTER, newTarget);
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


int16_t JointOrgan::readMeasuredAngle() {
	//Read and return the current angle, as measured by the potentiometer
	int number_of_retries=0;
	while(number_of_retries<NUMBER_OF_RETRIES_BEFORE_GIVING_UP) {
        if (write8To(MEASURED_POSITION_REGISTER, 0x00) == 0) {
            return read8();
        } else {
            printf("error during readMeasuredAngle (%d retries so far)\n",number_of_retries++);
        }
    }
    return -1; // reached max retries
}

uint8_t JointOrgan::readTestRegister() {
    //undocumented test register
    write8To(0x99, 0x00);
    return read8();
}



