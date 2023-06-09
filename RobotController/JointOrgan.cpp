/**
	@file JointOrgan.cpp
	@brief Implementation of JointOrgan methods (based on Mike's MotorOrgan)
        @author Matt / Mike
*/

#include "JointOrgan.hpp"



//Constructor
JointOrgan::JointOrgan(uint8_t address) : Organ(address){
	//Just uses the I2CDevice constructor
    
    // record organ type:
    organType = JOINT;
}

//new Target is a signed value in degrees for the target angle to move to
void JointOrgan::setTargetAngle(int8_t newTarget) {
    if(DEBUG_PRINT){std::cout<<"Setting target angle to: "<<newTarget<<std::endl;}
    if ( newTarget != targetPostion){
        targetPostion = newTarget;
        write8To(TARGET_POSITION_REGISTER, newTarget); //Send to device (write8To inherited from I2CDevice)
    }
}

//newValue is brightness, 0-255
void JointOrgan::setLEDBrightness(int8_t newValue) {
    //Send to device (write8To inherited from I2CDevice)
    write8To(LED_BRIGHTNESS_REGISTER, newValue);
}


//new Target is a signed float between -1 and 1, which will be converted to angle of +/-MAX_MAGNITUDE_TARGET_ANGLE degrees
void JointOrgan::setTargetAngleNormalised(float newTargetNormalised) {
    if (newTargetNormalised>1){
        setTargetAngle(MAX_MAGNITUDE_TARGET_ANGLE);
    }
    else if (newTargetNormalised<-1){
        setTargetAngle(-MAX_MAGNITUDE_TARGET_ANGLE);
    }
    else{
        setTargetAngle(MAX_MAGNITUDE_TARGET_ANGLE*newTargetNormalised);
    }
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


uint8_t JointOrgan::readMeasuredCurrent() {
    return read8From(MEASURED_CURRENT_REGISTER);
}

//NOTE: Must have a delay between using this function and further writes to I2C, therefore this function includes a sleep of 100ms.
//I2C to the joint organ is disabled while this executes, and if interrupted can lead to loss of comms
void JointOrgan::setCurrentLimit(uint8_t tensOfMilliamps){
    std::cout<<"Joint "<< unsigned(devAddress)<<" setting current limit to "<<unsigned(tensOfMilliamps)*10<<"mA... ";
    write8To(CURRENT_LIMIT_REGISTER, tensOfMilliamps);
    usleep(100000);
    if (testConnection()){std::cout<<"OK"<<std::endl;}
    else{std::cout<<"failed"<<std::endl;}
}




