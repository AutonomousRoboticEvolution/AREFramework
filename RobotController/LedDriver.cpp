/**
	@file LedDriver.cpp
	@brief Implementation of LedDriver methods
	@author Mike Angus
*/

#include "LedDriver.hpp"

//LEDDriver Constructor
LedDriver::LedDriver(uint8_t address) : I2CDevice(address) {
	//Calls the I2CDevice constructor
}

//LED driver initialisation function. Sets up the chip and the local member variables.
void LedDriver::init() {
	//Initialise/reset local LED parameters: ID, colour proportions, brightness, PWM values and Mode
	rgbLeds[0] = {RGB0, 0, 0, 0, 0, 0, 0, 0, FULL_OFF};
	rgbLeds[1] = {RGB1, 0, 0, 0, 0, 0, 0, 0, FULL_OFF};
	rgbLeds[2] = {RGB2, 0, 0, 0, 0, 0, 0, 0, FULL_OFF};
	rgbLeds[3] = {RGB3, 0, 0, 0, 0, 0, 0, 0, FULL_OFF};

	//Set MODE1 and MODE2 registers to start values defined in header
	write8To(LEDDRIVER_MODE1_REG, LEDDRIVER_MODE1_DEFAULT);
	usleep(500); //500us wait time for oscillator. p16 s9.5.1 in TLC59116 manual
	write8To(LEDDRIVER_MODE2_REG, LEDDRIVER_MODE2_DEFAULT);

	//Setup LED blinking defaults. These settings will apply to any LED set to BLINK mode
	setBlinkRateMs(LEDDRIVER_DEFAULT_BLINK_RATE_MS);
	setBlinkDutyCycle(LEDDRIVER_DEFAULT_BLINK_DUTY_CYCLE);

	//Reset the LED mode so that all are off
	for (int i=0; i<4; ++i) {
		setMode(rgbLeds[i].id, FULL_OFF, ALL);
	}
}

//Driver mode setting, for master on-off or enabling/disabling blinking
void LedDriver::setMode(ledId targetLed, ledMode newMode, subLed targetSubLed) {
	
	rgbLed* led = &rgbLeds[targetLed];	//Pointer to LED params struct

	//Target device address can be derived as an offset from the first register
	//since the registers are adjacent on the device
	uint8_t targetAddress = LEDDRIVER_OUTPUT_STATE_REG_0 + targetLed;
	uint8_t regState = 0;
	
	//Sub-leds within an RGB led are two-bit fields within an 8-bit register
	//If we're not changing all of them, we need to refer to the current state
	//since the whole register will be overwritten on the device
	if (targetSubLed != ALL) {
		regState = led->outputMode; //Get current register state to modify
		regState &= ~(0x3 << targetSubLed*2);	//Clear the relevant two bits from register
		regState |= (newMode << targetSubLed*2);	//Write the new mode bits for sub led
	} else {
		//Write new mode bits for all subleds. Note that we are only using 3 of 4 possible per channel
		regState = newMode << 4 | newMode << 2 | newMode;
	}

	//Output the new state
	led->outputMode = regState;	//Save local copy of register state
	write8To(targetAddress, regState);	//Write new state register to device
}

//LED colour setting; use to switch to predefined colours, can also be used to turn an LED off
void LedDriver::setColour(ledId targetLed, ledColour colour) {
	
	rgbLed* led = &rgbLeds[targetLed];	//Pointer to LED params struct

	//Use of this function implies a desire for LED to be on, unless 2nd arg is 'OFF'
	//If so, use default brightness unless nonzero value already set previously
	if (led->brightness == 0 && colour != OFF) {
		led->brightness = LEDDRIVER_DEFAULT_BRIGHTNESS;
	}
	
	//Enable RED for red, yellow, magenta or white, otherwise disable
	if (colour == RED || colour == YELLOW || colour == MAGENTA || colour == WHITE) {
		led->redPwm = led->brightness;
		led->redProportion = 1.0;
	} else {
		led->redPwm = 0;
		led->redProportion = 0.0;
	}

	//Enable GREEN for green, yellow, cyan or white, otherwise disable
	if (colour == GREEN || colour == YELLOW || colour == CYAN || colour == WHITE) {
		led->greenPwm = led->brightness;
		led->greenProportion = 1.0;
	} else {
		led->greenPwm = 0;
		led->greenProportion = 0.0;
	}

	//Enable BLUE for blue, cyan, magenta or white, otherwise disable
	if (colour == BLUE || colour == CYAN || colour == MAGENTA || colour == WHITE) {
		led->bluePwm = led->brightness;
		led->blueProportion = 1.0;
	} else {
		led->bluePwm = 0;
		led->blueProportion = 0.0;
	}

	//If the given 'colour' was OFF, all above proportions/pwms will be zero and LED will turn off.
	//Local brightness setting will persist, however, so LED will remember previous brightness when next turned on


	//Update the PWM with the new settings
	updatePwm(targetLed);
}

//Use to set arbitrary RGB colours by specifying proportions
void LedDriver::setColourRgb(ledId targetLed, float red, float green, float blue) {
	
	rgbLed* led = &rgbLeds[targetLed];	//Pointer to LED params struct

	//Set the brightness to default if currently zero, since colour has no meaning without it
	//Will otherwise adopt the brightness level that was already set
	if (led->brightness == 0) {
		led->brightness = LEDDRIVER_DEFAULT_BRIGHTNESS;
	}

	//Update the colour proportions
	led->redProportion = red;
	led->greenProportion = green;
	led->blueProportion = blue;

	//Set the brightness of the sub LEDs accordingly
	setBrightness(targetLed, led->brightness);
	
}

//Set the brightness level of specified RGB LED
void LedDriver::setBrightness(ledId targetLed, uint8_t brightness) {
	
	rgbLed* led = &rgbLeds[targetLed];	//Pointer to LED params struct

	//Update the brightness parameter
	led->brightness = brightness;
	//Derive the relative colour brightnesses from the given brightness with current proportions
	led->redPwm = (uint8_t)(led->redProportion * (float) brightness);
	led->greenPwm = (uint8_t)(led->greenProportion * (float) brightness);
	led->bluePwm = (uint8_t)(led->blueProportion * (float) brightness);

	//Update the PWM with the new brightness
	updatePwm(targetLed);
}

//Set the blink rate for all LEDs with blinking enabled
void LedDriver::setBlinkRateMs(int blinkRateMs) {

	uint8_t grpfreq = (24*blinkRateMs)/1000 - 1; //From TLC59116 datasheet p18
	write8To(LEDDRIVER_GRPFREQ_REG, grpfreq);
}

//Set the blink duty cycle for all LEDs with blinking enabled
void LedDriver::setBlinkDutyCycle(float dutyCycle) {

	uint8_t grppwm = 256 * dutyCycle; //From TLC59116 datasheet p17
	write8To(LEDDRIVER_GRPPWM_REG, grppwm);
}

void LedDriver::setAllTo(ledColour color, int brightness) {
    ledId leds[4] = {RGB0, RGB1, RGB2, RGB3};
    for (int i=0; i<4; ++i) { // loop through and turn on all the LEDS
        this->setMode(leds[i], PWM, ALL);
        this->setColour(leds[i],color);
        this->setBrightness(leds[i], brightness);
    }
}

void LedDriver::flash(ledColour colour, int time , int brightness){
    setAllTo(colour);
    usleep(time);
    this->setAllTo(OFF, brightness);
}

//Test the LED functions
void LedDriver::test() {

	printf("Testing TLC59116 LED Driver\n");

	//Initialise and setup
	init();
	setMode(RGB0, PWM, ALL);
    setMode(RGB1, PWM, ALL);
    setMode(RGB2, PWM, ALL);
    setMode(RGB3, PWM, ALL);

    //Parameters and test IDs
    int delayTimeUs = 150000;
    int fadeDelayTimeUs = 1960;	//~255 over 0.5s
    ledId leds[4] = {RGB0, RGB1, RGB2, RGB3};
    ledColour colours[8] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, OFF};

    //Cycle through all base colour combinations for each LED
    //Followed by colour crossfading
    for (int i=0; i<4; ++i) {
    	
    	//Cycle through preset colours
    	for (int m=0; m<8; ++m) {
    		setColour(leds[i], colours[m]);
    		usleep(delayTimeUs);
    	}

    	//Crossfade red via yellow to green
    	for (int k=0; k<256; ++k) {
    		float offset = (1.0/256)*k;
    		setColourRgb(leds[i],1.0-offset, 0.0+offset, 0);
    		usleep(fadeDelayTimeUs);
    	}
    	//Crossfade green via cyan to blue
    	for (int k=0; k<256; ++k) {
    		float offset = (1.0/256)*k;
    		setColourRgb(leds[i],0, 1.0-offset, 0.0+offset);
    		usleep(fadeDelayTimeUs);
    	}

    	//Brightness fadeout
    	for (int j=255; j>=0; --j) {
    		setBrightness(leds[i], j);
    		usleep(fadeDelayTimeUs);
    	}
    }

    //Blinking test
    int blinkRate = 100;
    setBlinkRateMs(blinkRate);
    setBlinkDutyCycle(0.1);
    for (int i=0; i<4; ++i) {
    	setMode(leds[i], BLINK, ALL);
    	setColour(leds[i], WHITE);
    	setBrightness(leds[i], 0);
    }

    //Fade in
    for (int j=0; j<256; ++j) {
    	for (int i=0; i<4; ++i) {
    		setBrightness(leds[i], j);
    		usleep(fadeDelayTimeUs);
    	} 		
    }

    //Fade out
    for (int j=255; j>=0; --j) {
    	for (int i=0; i<4; ++i) {
    		setBrightness(leds[i], j);
    		usleep(fadeDelayTimeUs);
    	} 		
    }

    //Reset
    init();
    
    printf("Finished testing LED driver\n");
}

//(Private) Write PWM values to the brightness registers on the device
void LedDriver::updatePwm(ledId targetLed) {
	
	rgbLed* led = &rgbLeds[targetLed];	//Pointer to LED params struct

	//Get address of first PWM register for this LED
	uint8_t baseAddress = LEDDRIVER_PWM_REG_0 + 4*led->id; //Registers are grouped in fours

	//Write PWM values to PWM registers. Using subLed enums to map to correct registers
	write8To(baseAddress + RED_SUB, led->redPwm);
	write8To(baseAddress + GREEN_SUB, led->greenPwm);
	write8To(baseAddress + BLUE_SUB, led->bluePwm);
}
