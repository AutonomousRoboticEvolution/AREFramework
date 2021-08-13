/**
	@file MotorOrgan.cpp
	@brief Implementation of MotorOrgan methods
	@author Mike Angus
*/

#include "MotorOrgan.hpp"

//Constructor
MotorOrgan::MotorOrgan(uint8_t address) : Organ(address){
	//Just uses the I2CDevice constructor

    // record organ type:
    organType = WHEEL;
}

//Speed input is a signed 8-bit integer where -ve value denotes reverse direction
void MotorOrgan::setSpeed(int8_t speed) {
	//Determine direction and hence set current state
	currentState = (speed > 0) ? FORWARD : REVERSE;

	//Set current speed
	currentSpeed = abs(speed);

	//Constrain to maximum representable value (6 bits; 2^6 = 63)
    //Speeds correspond to target velocity, which the wheel organ firmware will attempt to achieve.
	if (currentSpeed > 63) {
		currentSpeed = 63;
	}

	//Write current values to control register
	writeControlReg();
}

//Speed input is a signed float, where -1 is maximum reverse and +1 is maximum forward
void MotorOrgan::setSpeedNormalised(float speed) {
    // constain the value to be within the expected range of -1 to +1:
    if (speed>1.0){speed=1.0;}
    else if(speed<-1.0){speed=-1.0;}

    setSpeed(speed*MAXIMUM_TARGET_VELOICTY);

}

//Stop the motor using the braking feature
//up to 12 ms recovery time to full duty cycle
void MotorOrgan::brake() {
	//Set current speed to zero and set state to brake
	currentSpeed = 0;
	currentState = BRAKE;

	//Write current values to control register
	writeControlReg();
}

//Stop the motor and place in low-power mode
//up to 12 ms recovery time to full duty cycle
void MotorOrgan::standby() {
	//Set current speed to zero and state to standby
	currentSpeed = 0;
	currentState = STANDBY;

	//Write current values to control register
	writeControlReg();
}

//NOTE: Must have a delay between using this function and further writes to I2C
//I2C to the joint organ is disabled while this executes, and if interrupted can lead to loss of comms
void MotorOrgan::setCurrentLimit(uint8_t tensOfMilliamps){
    write8To(CURRENT_LIMIT_REGISTER, tensOfMilliamps);
}

int8_t MotorOrgan::readMeasuredCurrent() {
    return read8From(GET_MEASURED_CURRENT_REGISTER);
}

int8_t MotorOrgan::readMeasuredVelocity() {
    return read8From(GET_MEASURED_VELOCITY_REGISTER);
}

float MotorOrgan::readMeasuredVelocityRPM() {
    return float(this->readMeasuredVelocity())*CONVERT_ENCODER_TICKS_PER_TIMESTEP_TO_REV_PER_MINUTE;
}

/**PRIVATE FUNCTIONS ***/
//Write current speed and state values to control register
void MotorOrgan::writeControlReg() {
	//Prepare contents of control register
	uint8_t ctrlByte = currentSpeed << 2 | currentState;

	//Send to device (write8To inherited from I2CDevice)
	write8To(DRV_CONTROL_REG_ADDR, ctrlByte);
}
