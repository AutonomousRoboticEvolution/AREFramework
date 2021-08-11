/**
	@file MotorOrgan.hpp
	@brief Class declaration for MotorOrgan
	@author Mike Angus
*/

#ifndef MOTORORGAN_HPP
#define MOTORORGAN_HPP

#include <cstdlib> //for abs() function
#include "I2CDevice.hpp"
#include "DaughterBoards.hpp"

#define SET_TEST_VALUE_REGISTER 0x90 // save a given value
#define GET_TEST_VALUE_REGISTER 0x91 // return the saved value

//Register subaddresses for DRV8830 chip
#define DRV_CONTROL_REG_ADDR 0x00
#define DRV_FAULT_REG_ADDR 0x01

//IN1 and IN2 states
#define STANDBY 0x0
#define REVERSE 0x1
#define FORWARD 0x2
#define BRAKE 0x3

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
class MotorOrgan  : protected I2CDevice {
	public :

		//Public variables
		//Vars for motor state and speed
		uint8_t currentState = STANDBY; ///< Motor state: STANDBY, REVERSE, FORWARD or BRAKE, defined in MotorOrgan.hpp
		uint8_t currentSpeed = 0; ///< Magnitude of motor speed, unsigned 6 bit value, so 0-63 range. Direction is given by currentState

        boardSelection daughterBoardToEnable = BOTH; // to store which daughterboard this organ is attached to, so must be enabled before use

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
			@brief Read the contents of the fault register.
			@return One byte containing the fault register bits, which indicate whether a
			fault has occurred, and of what type. Further information in the datasheet.
			\code
			FAULT REGISTER BIT MEANINGS
			________________________________________
			 CLEAR --- --- ILIMIT OTS UVLO OCP FAULT
			   7	6   5     4    3    2   1    0

			 CLEAR: Write '1' to this to clear fault reg
			 ILIMIT: Current limit exceeded event
			 OTS: Overtemperature shutdown
			 UVLO: Undervoltage lockout
			 OCP: Overcurrent protection
			 FAULT: Any fault condition exists
			\endcode
		*/
		uint8_t readFaultReg();

		/**
			@brief Clear all the bits in the fault register.
			This can be used to clear errors in the case of a fault which stops operation.
		*/
		void clearFaultReg();

        bool test();

        void set_test_value(uint8_t value);
        uint8_t get_test_value();

	private :
		/**
			@brief Update the CONTROL register with the current state and speed.
			Generates a byte from currentSpeed and currentState, then writes to CONTROL register.
		*/
		void writeControlReg();
};

#endif
