/**
	@file MotorOrgan.hpp
	@brief Class declaration for MotorOrgan
	@author Mike Angus
*/

#ifndef MOTORORGAN_HPP
#define MOTORORGAN_HPP

#include <cstdlib> //for abs() function
#include "Organ.hpp"

// The demand wheel velocity, in ticks-per-timestep, which corresponds to a normalised input of 1.0
// note to convert from ticks-per-timestep to revolutions-per-minute (rpm), multiply by 1.67785
// e.g. 36 ticks-per-timestep = 60.4rpm
#define MAXIMUM_TARGET_VELOICTY 36

//Register subaddresses
#define DRV_CONTROL_REG_ADDR 0x00
#define GET_MEASURED_VELOCITY_REGISTER 0x12
#define GET_MEASURED_CURRENT_REGISTER 0x13
#define CURRENT_LIMIT_REGISTER 0x14

//IN1 and IN2 states
#define STANDBY 0x0
#define REVERSE 0x1
#define FORWARD 0x2
#define BRAKE 0x3

#define CONVERT_ENCODER_TICKS_PER_TIMESTEP_TO_REV_PER_MINUTE 1.677852349

//CONTROL register fields
// [7:2] motorSpeed bits
// [1:0] motorState bits

/**
	Controller class for the DRV8830 I2C motor driver chip.
	Methods are provided for setting the motor speed, 
	as well as reading and resetting the fault detection register. 
	
	The motor driver differs from an ordinary h-bridge in that it produces a regulated 
	voltage output rather than just a proportion of the supply voltage. 
	This means that, for example, a speed setting of 0x1F should result in an output of
	 2.49V regardless of the power supply voltage, unless the power supply is itself
	  at a lower voltage than this. The maximum output voltage value is 5.06V

	Details of the driver chip can be found at: http://www.ti.com/lit/ds/symlink/drv8830.pdf

	The address of the device is set using the DIP switches on the PCB. There are 
	two address pins A0 and A1, which can each be in one of three states: 
	Vcc, GND, or Open, enabling a total of 9 unique addresses. The DIP switches 
	are wired as follows:

	\code
	Vcc GND Vcc GND
	 |   |   |   |
	 /   /   /   /
	 |   |   |   |
	 1   2   3   4
	  \ /     \ /
	   A1      A0
	\endcode

	Details of how this produces different addresses are given in the datasheet, 
	but note that these are given in 8-bit format and need to be right-shifted by 
	1 bit to get into the correct form for usage with this library. This issue is
	explained in the I2CDevice class description. A more convenient guide to address-
	setting with the DIP switches is contained on Drive under 20.50 User Documentation 
	- "Motor Organ I2C Address Setting.pdf"
*/
class MotorOrgan  : public Organ {
	public :

		//Public variables
		//Vars for motor state and speed
		uint8_t currentState = STANDBY; ///< Motor state: STANDBY, REVERSE, FORWARD or BRAKE, defined in MotorOrgan.hpp
		uint8_t currentSpeed = 0; ///< Magnitude of motor speed, unsigned 6 bit value, so 0-63 range. Direction is given by currentState

		//Public member functions
		/**
			@brief MotorOrgan constructor.
			This simply calls the superclass constructor I2CDevice()
			@param address The I2C device address, using 7-bit version
			 (see I2CDevice class description for explanation)
		*/
		MotorOrgan(uint8_t address);

		/**
			@brief Speed controlling method.
			@param speed Signed 6-bit integer. Negative sign means reverse direction.
			Maximum magnitude is 2^6 = 63; values in excess of this are 
			automatically constrained to 63. Note that maximum motor speed may be 
			reached at values less than 63, depending on the supply voltage. 
			There will also be a certain minimum value required in order to 
			overcome the stall torque of the motor, which will depend on how 
			the motor is loaded at the time.
		*/
		void setSpeed(int8_t speed);

        /**
            @brief Way to call the speed controlling method with a normalised input. Will convert the input value to the correct range and then call setSpeed().
            @param speed Float. Expected input range is -1 to +1, values outside this range will be capped.
        */
        void setSpeedNormalised(float speed);

		/**
			@brief Stop the motor using the braking feature.
			This will set the speed to zero and the motor state to BRAKE.
			Afterwards, there will be a small delay in ramping up to speed again. This can
			take up to 12ms to reach full duty cycle on the output.
		*/
		void brake();

		/**
			@brief Stop the motor and enter low-power standby mode.
			This will set the speed to zero and the motor state to STANDBY.
			Afterwards, there will be a small delay in ramping up to speed again. This can
			take up to 12ms to reach full duty cycle on the output.
		*/
		void standby();

        /**
            @brief Set the current limit value
            @param tensOfMilliamps the desired limit, in tens of milliams (i.e. 100 = 1A)

            NOTE: Must have a delay between using this function and further writes to I2C
            I2C to the joint organ is disabled while this executes, and if interrupted can lead to loss of comms
        */
        void setCurrentLimit(uint8_t tensOfMilliamps);

        /**
            @brief Read the instantaneous current draw.
            @return
        */
        int8_t readMeasuredCurrent();

        /**
            @brief Read the instantaneous rotational velocity of the wheel.
            @return the measured veloicty, in encoder ticks per timestep
        */
        int8_t readMeasuredVelocity();

        /**
            @brief Read the instantaneous current draw.
            @return the measured veloicty, in revolutions per minute
        */
        float readMeasuredVelocityRPM();


	private :
		/**
			@brief Update the CONTROL register with the current state and speed.
			Generates a byte from currentSpeed and currentState, then writes to CONTROL register.
		*/
		void writeControlReg();
};

#endif
