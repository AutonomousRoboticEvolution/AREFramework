/**
	@file JointOrgan.hpp
	@brief Class declaration for JointOrgan (based on Mike's MotorOrgan)
	@author Matt
*/

#ifndef JOINTORGAN_HPP
#define JOINTORGAN_HPP

#define DEBUG_PRINT false

#define MAX_MAGNITUDE_TARGET_ANGLE 80 // the maximum target position in degrees, where zero is straight, e.g. a value of 90 would give a total range of 180 degrees

//#include <cstdlib> //for abs() function
#include "Organ.hpp"
#include <iostream> // for cout debugging

//Register addresses
#define SERVO_ENABLE_REGISTER 0X10
#define TARGET_POSITION_REGISTER 0x11
#define MEASURED_POSITION_REGISTER 0x12
#define MEASURED_CURRENT_REGISTER 0x13
#define CURRENT_LIMIT_REGISTER 0x14
#define LED_BRIGHTNESS_REGISTER 0x15

#define SERVO_OFF 0
#define SERVO_ON 1

/**

	Controller class for the arduino-pico based servo controller in the joint organ
*/
class JointOrgan  : public Organ {
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
            @brief Target angle setting method. Will automatically activate the servo
            @param target angle is a signed value in degrees
		*/
        void setTargetAngle(int8_t newTarget);

        /**
            @brief LED brightness setting method.
            @param value for LED brightness, 0-255, will be the PWM value set by the arduino
        */
        void setLEDBrightness(int8_t newValue);
        
		/**
            @brief Target angle setting method for input directly from the high level controller
            @param target angle is a signed float between -1 and 1. This will correspond to angles from -MAX_MAGNITUDE_TARGET_ANGLE to MAX_MAGNITUDE_TARGET_ANGLE (i.e. plus/minus 80 degrees)
		*/
        void setTargetAngleNormalised(float newTargetNormalised);

        /**
            @brief Set the current limit value
            @param tensOfMilliamps the desired limit, in tens of milliams (i.e. 100 = 1A)
            
            NOTE: Must have a delay between using this function and further writes to I2C, therefore this function includes a sleep of 100ms.
			I2C to the joint organ is disabled while this executes, and if interrupted can lead to loss of comms
        */
        void setCurrentLimit(uint8_t tensOfMilliamps);

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
        int8_t readMeasuredAngle();


        /**
            @brief Read the current draw.
            @return
        */
        int8_t readMeasuredCurrent();

};


#endif
