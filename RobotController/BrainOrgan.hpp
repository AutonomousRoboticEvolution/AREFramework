/**
	@file BrainOrgan.hpp
	@brief Class declarations for BrainOrgan and functionality local to the brain board
	@author Mike Angus
*/

#ifndef BRAINORGAN_HPP
#define BRAINORGAN_HPP

#include "I2CDevice.hpp"

//Definitions for LED Driver
//MODE1 register default should be 0000001
//Critical is that bit [4] is set to 0 to enable the oscillator
#define LEDDRIVER_MODE1_DEFAULT 0x01
//MODE2 register default should be 00101000
//Bit [5] enables global blinking control (instead of global brightness control)
//Bit [3] sets outputs to change immediately on ACK instead of waiting for Stop command
#define LEDDRIVER_MODE2_DEFAULT 0x28
//Blinking frequency and duty cycle
#define LEDDRIVER_DEFAULT_BLINK_RATE_MS 150
#define LEDDRIVER_DEFAULT_BLINK_DUTY_CYCLE 0.3f

//Default PWM brightness
#define LEDDRIVER_DEFAULT_BRIGHTNESS 0xFF

//Register addresses
#define LEDDRIVER_MODE1_REG 0x00
#define LEDDRIVER_MODE2_REG 0x01
#define LEDDRIVER_GRPPWM_REG 0x12
#define LEDDRIVER_GRPFREQ_REG 0x13
#define LEDDRIVER_OUTPUT_STATE_REG_0 0x14 //base address for series of output state registers
#define LEDDRIVER_PWM_REG_0 0x02 //base address for series of PWM registers

/**
	ledId is used throughout LedDriver to identify RGB LEDs on the motherboard.
	These names match those printed on the PCB itself.
	RGB0 = left-front
	RGB1 = left-rear
	RGB2 = rear-left
	RGB3 = rear-right
	The numerical order of IDs here corresponds to how the RGB LEDs are connected to the chip.	
*/
enum ledId {RGB0, RGB1, RGB2, RGB3};
/**
	ledMode is used by LedDriver::setMode to change output mode of each sub-LED (individual R,G,B)
	FULL_OFF = Driver is inactive and LED will not light
	FULL_ON = Driver is completely on and LED will be illuminated at full power, ignoring blinking and PWM 
	PWM = Driver is active and LED will illuminate at a PWM-controlled brightness, allowing dimming and colour mixing
	BLINK = Driver is active and responding to PWM, but LED will also flash at the global blink frequency. 
	The binary values of the constants here are the actual 2-bit values written to the device for each mode.
*/
enum ledMode {FULL_OFF, FULL_ON, PWM, BLINK};
/**
	subLed is used by LedDriver::setMode to address sub-LEDs; i.e. the R, G and B LEDs comprising each RGB unit.
	GREEN_SUB = green LED only
	RED_SUB = red LED only
	BLUE_SUB = blue LED only
	ALL = all three LEDs
	The order of the three colours here corresponds to how the sub-LEDs are arranged in each RGB unit and connected to the chip.
*/
enum subLed {GREEN_SUB, RED_SUB, BLUE_SUB, ALL};
/**
	ledColour is used by LedDriver::setColour to provide aliases for the basic set of colours.
	RED, GREEN, BLUE = self-explanatory
	YELLOW = RED+GREEN
	CYAN = GREEN+BLUE
	MAGENTA = RED+BLUE
	WHITE = RED+GREEN+BLUE
	OFF = all colours set to 0
*/
enum ledColour {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, OFF};

/**
	LED control data structure; each instance identified by 'id'
	The ratio of red:green:blue proportions sets the colour
	This is then scaled by the brightness value to calculate the PWM values
	The output mode is expected to be PWM or BLINK when in use
*/
struct rgbLed {
	ledId id;
	float redProportion;	//0.0-1.0
	float greenProportion;	//0.0-1.0
	float blueProportion;	//0.0-1.0
	uint8_t brightness;
	uint8_t redPwm;
	uint8_t greenPwm;
	uint8_t bluePwm;
	uint8_t outputMode;
};

/**
	LedDriver provides functions for controlling RGB LEDs using the TLC59116 I2C LED driver.
	Datasheet here: https://www.ti.com/lit/gpn/TLC59116
*/
class LedDriver : protected I2CDevice {
	public :
		//Public member functions
		/**
			@brief LedDriver constructor.
			This simply calls the superclass constructor I2CDevice()
			@param address The I2C device address, using 7-bit version
			 (see I2CDevice class description for explanation)
		*/
		LedDriver(uint8_t address);

		/**
			@brief LED driver initialisation routine.
			Sets up local variables and sets up the device itself, ready to use.
			This must be run before LEDs will work.
			Note the outputs are off by default; LEDs must also be enabled with setMode() before they will do anything.
		*/
		void init();
		/**
			@brief Set the output mode of a particular RGB LED.
			Primarily for master enabling/disabling LEDs or setting them to blink.
			Colour changing, brightness and general on-off should be controlled using brightness and colour functions
			
			FULL_OFF = Driver is inactive and LED will not light
			FULL_ON = Driver is completely on and LED will be illuminated at full power, ignoring blinking and PWM 
			PWM = Driver is active and LED will illuminate at a PWM-controlled brightness, allowing dimming and colour mixing
			BLINK = Driver is active and responding to PWM, but LED will also flash at the global blink frequency.
			
			@param targetLed The ID of the RGB LED to be changed. Enum'd names are RGB0, RGB1, RGB2, RGB3
			@param newMode The mode to set the LED to. Options are FULL_OFF, FULL_ON, PWM, BLINK. 
			Note that ON mode has no brightness control, and BLINK mode is still dimmable/colourable as with PWM mode
			@param targetSubLed Use to control individual colours within the RGB LED, for example blinking
			only one of the colours to create a flashing colour change with another colour kept constant.
			Options are GREEN, RED, BLUE, ALL.
		*/
		void setMode(ledId targetLed, ledMode newMode, subLed targetSubLed);
		
		/**
			@brief Function to set the colour of an RGB LED to one of the predefined options.
			If brightness has not been set to a non-zero value, this will also turn the LED on by setting brightness to default.
			This provides all the primary and secondary colours, as well as WHITE (all on) and NONE (all off).
			For more granular mixing of colours, use setColourRgb.
			@param targetLed The ID of the RGB LED to be changed. Enum'd names are RGB0, RGB1, RGB2, RGB3
			@param colour The desired colour. Options are RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, OFF
		*/
		void setColour(ledId targetLed, ledColour colour);

		/**
			@brief Function to perform arbitrary colour mixing to get non-standard shades.
			These are defined as proportions from 0.0 - 1.0, which are used to scale the brightness value to control the sub-LEDs
			If brightness has not been set to a non-zero value, this will also turn the LED on by setting brightness to default.
			@param targetLed The ID of the RGB LED to be changed. Enum'd names are RGB0, RGB1, RGB2, RGB3
			@param red The proportion of red light in the mix, between 0.0 (none) to 1.0 (equal to main brightness value)
			@param green The proportion of green light in the mix, between 0.0 (none) to 1.0 (equal to main brightness value)
			@param blue The proportion of blue light in the mix, between 0.0 (none) to 1.0 (equal to main brightness value)
		*/
		void setColourRgb(ledId targetLed, float red, float green, float blue);


		/**
			@brief Function to set the brightness of an RGB LED
			This is a value from 0-255; which is then scaled by the colour proportions to produce a dimmed LED of the desired colour.
			@param targetLed The ID of the RGB LED to be changed. Enum'd names are RGB0, RGB1, RGB2, RGB3
			@param brightness The desired brightness between 0 (off) and 255 (very bright)
		*/
		void setBrightness(ledId targetLed, uint8_t brightness);

		/**
			@brief Set the blinking time period that LEDs will use in blinking mode.
			This is a global setting and cannot be applied in an LED-specific way. If independent blink rates are needed, that must be timed from software.
			@param blinkRateMs The time period of blinking in milliseconds. The range is 41ms to 10730ms.
		*/
		void setBlinkRateMs(int blinkRateMs);

		/**
			@brief Set the blink duty cycle that LEDs will use in blinking mode. This is a global setting.
			@param dutyCycle The duty cycle expressed as a value between 0.0 and 1.0. Higher values mean longer flashes, with 1.0 being on continuously.
		*/
		void setBlinkDutyCycle(float dutyCycle);

		/**
			@brief Runs a test illumination sequence on the LEDs.
			Each LED should cycle through all the base colours and do a colour crossfade from R->G->B.
			Finishes by flashing all LEDs while dimming the brightness up and down.
		*/
		void test();

	private:
		//Array of rgbLed structs. These hold all local information about the individual LED settings.
		rgbLed rgbLeds[4];

		/**
			@brief Writes local PWM values to the TLC59116 to update the LED control
			@param targetLed The ID of the RGB LED to be updated
		*/
		void updatePwm(ledId targetLed);
};

#endif