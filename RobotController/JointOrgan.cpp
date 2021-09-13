/**
	@file JointOrgan.cpp
	@brief Implementation of JointOrgan methods (based on Mike's MotorOrgan)
        @author Matt / Mike
*/

#include "JointOrgan.hpp"



//Constructor
JointOrgan::JointOrgan(uint8_t address) : Organ(address){
	//Just uses the I2CDevice constructor
}

//new Target is a signed value in degrees for the target angle to move to
void JointOrgan::setTargetAngle(int8_t newTarget) {
	//Send to device (write8To inherited from I2CDevice)
    write8To(TARGET_POSITION_REGISTER, newTarget);
}

//Turn off servo so it should move freely
void JointOrgan::setServoOff() {
    write8To(SERVO_ENABLE_REGISTER, false);

}

// Enable/disable servo power
void JointOrgan::setServoOn() {
        write8To(SERVO_ENABLE_REGISTER, true);
}


int8_t JointOrgan::readMeasuredAngle() {
    return read8From(MEASURED_POSITION_REGISTER);
}


int8_t JointOrgan::readMeasuredCurrent() {
    return read8From(MEASURED_CURRENT_REGISTER);
}

//NOTE: Must have a delay between using this function and further writes to I2C
//I2C to the joint organ is disabled while this executes, and if interrupted can lead to loss of comms
void JointOrgan::setCurrentLimit(uint8_t tensOfMilliamps){
    write8To(CURRENT_LIMIT_REGISTER, tensOfMilliamps);
}




