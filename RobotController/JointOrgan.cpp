/**
	@file JointOrgan.cpp
	@brief Implementation of JointOrgan methods (based on Mike's MotorOrgan)
        @author Matt / Mike
*/

#include "JointOrgan.hpp"
#include <iostream> // for cout debugging



//Constructor
JointOrgan::JointOrgan(uint8_t address) : I2CDevice(address){
	//Just uses the I2CDevice constructor
}

//new Target is an 8-bit number representing angle in degrees (0 to 180)
void JointOrgan::setTargetAngle(uint8_t newTarget) {
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

void JointOrgan::setCurrentLimit(uint8_t tensOfMilliamps){
    write8To(CURRENT_LIMIT_REGISTER, tensOfMilliamps);
}

void JointOrgan::testFunction(){
    std::cout<<"Testing the joint.."<<std::endl;

    std::cout<<"Power On"<<std::endl;
    this->setServoOn();

    for (uint8_t limit_value=40; limit_value<150; limit_value+=20){
        std::cout<<"New limit: "<<int(limit_value)<<std::endl;

        this->setCurrentLimit(limit_value);
	sleep(1); //Sleep to avoid interrupting while ext i2c is turned off
        this->setTargetAngle(45);
        sleep(1);
        this->setTargetAngle(135);
        sleep(1);

    }


}



