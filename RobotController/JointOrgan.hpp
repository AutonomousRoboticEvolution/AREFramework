/**
	@file JointOrgan.hpp
	@brief Class declaration for JointOrgan (based on Mike's MotorOrgan)
	@author Matt
*/

#ifndef JOINTORGAN_HPP
#define JOINTORGAN_HPP


//#include <cstdlib> //for abs() function
#include "I2CDevice.hpp"

//Register subaddresses for DRV8830 chip
#define TARGET_POSTIION_REGISTER 0x10
#define JOINT_MODE_REGISTER 0X11
#define MEASURED_POSITION_REGISTER 0x12

//the options for JOINT_MODE_REGISTER
#define SERVO_OFF 0x00
#define SERVO_ON 0x01


/**

	Controller class for the arduino-pico based servo controller in the joint organ
*/
class JointOrgan  : protected I2CDevice {
	public :
		//Public variables
		//Vars for state and target position
		uint8_t currentState = SERVO_OFF; ///< Servo state: SERVO_OFF or SERVO_ON
		uint8_t targetPostion = 128; ///< target angle, as an 8-bit number (0 to 255) representing angles from 0degrees to 180degrees

		//Public member functions
		/**
			@brief JointOrgan constructor.
			This simply calls the superclass constructor I2CDevice()
			@param address The I2C device address, using 7-bit version
			 (see I2CDevice class description for explanation)
		*/
		JointOrgan(uint8_t address);

		/**
			@brief Target angle setting method. Will automatically active the servo
			@param target angle is an unsigned 8-bit integer, linearly spaced over the possible range of approximately 180 degrees.
		*/
		void setTargetAngle(uint8_t newTarget);

		/**
			@brief Turn the servo off so it should turn freely (and draw no power)
		*/
		void setServoOff();

		/**
			@brief Turn the servo on so it tries to acheive the target position
		*/
		void setServoOn();


		/**
			@brief Read the current angle of the servo.
			@return
		*/

        int16_t readMeasuredAngle();
		uint8_t readTestRegister(); // undocumented test register


};


#endif
