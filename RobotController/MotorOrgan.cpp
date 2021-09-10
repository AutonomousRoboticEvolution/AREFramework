/**
	@file MotorOrgan.cpp
	@brief Implementation of MotorOrgan methods
	@author Mike Angus
*/

#include "MotorOrgan.hpp"

//Constructor
MotorOrgan::MotorOrgan(uint8_t address) : Organ(address){
	//Just uses the I2CDevice constructor
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

//Read the contents of the FAULT register
/* FAULT REG BIT MEANINGS
________________________________________
 CLEAR --- --- ILIMIT OTS UVLO OCP FAULT
   7	  6   5     4    3    2   1    0

 CLEAR: Write '1' to this to clear fault reg
 ILIMIT: Current limit exceeded event
 OTS: Overtemperature shutdown
 UVLO: Undervoltage lockout
 OCP: Overcurrent protection
 FAULT: Set if any fault condition exists
*/
uint8_t MotorOrgan::readFaultReg() {
	//Read and return the contents of the fault register
	return read8From(DRV_FAULT_REG_ADDR);
}

//Clear the contents of the FAULT register
void MotorOrgan::clearFaultReg()  {
	//Write '1' to bit [7] of the fault register to clear it
	uint8_t clear = 0x80; //1000000
	write8To(DRV_FAULT_REG_ADDR, clear);
}

/**PRIVATE FUNCTIONS ***/
//Write current speed and state values to control register
void MotorOrgan::writeControlReg() {
	//Prepare contents of control register
	uint8_t ctrlByte = currentSpeed << 2 | currentState;

	//Send to device (write8To inherited from I2CDevice)
	write8To(DRV_CONTROL_REG_ADDR, ctrlByte);
}
