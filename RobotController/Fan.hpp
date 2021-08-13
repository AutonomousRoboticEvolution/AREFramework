/**
	@file Fan.hpp
	@brief Header and documentation for Fan.cpp, for running the case fan
	@author Matt Hale
*/

#ifndef FAN_HPP
#define FAN_HPP

/**
 * Note, in order to use the PWM output we need sudo privileges, so this can be set to false when sudo not used.
 * If this is set false, any positive value will set the fan to full, zero (or negative) will turn off
 * This will result in a bang-bang thermostat style controller instead of proportional.
 */
#define USE_PWM_OUTPUT false

#define FAN_GPIO_PIN 18

/**
 * Gain for temperature controller. Still need to be tuned. Computed as:
 * fan_PWM_value =  K_OFFSET + K_P*temperature_error if the temperature is too high, zero otherwise
 * units is change in PWM (FAN_PWM_MIN=off, FAN_PWM_MAX=max speed) value per degreeC for K_P
 */
#define K_OFFSET 200
#define K_P 100 // so an error of ~8degC will result in fan at full speed - seems reasonable?

#define FAN_PWM_MIN 0 // this is the PWM value for off
#define FAN_PWM_MAX 1024 // this is the PWM value for fully on

#include <wiringPi.h>

class Fan{
    public:
        Fan();

        void init(bool usePWM);
        /**
         * quick demo, turn fan on for a few seconds and then off
         */
        void test();

        /**
         * set fan to max speed
         */
        void turnOn();

        /**
         * set fan to zero
         */
        void turnOff();

        /**
         * set fan speed, using the PWM function of wiringPi
         * @param value the PWM value to use, 0 (or below) is off, 1024 (or above) is max speed
         */
        void setPWMValue(int value);

        /**
         * set the target temperature for the simple controller
         * @param value the temperature, in degreesC, which the fan will turn on at
         */
        void setTargetTemperature(float value);

        /**
         * update the simple controller. You need to find out what the current temperature is separately, e.g. batteryMonitor.measureTemperature()
         * @param currentTemperature the temperature right now, in degreesC
         */
        void update(float currentTemperature);

        /**
        * used to remember what the target temperature is between calls to setTargetTemperature() and update()
        */
        float targetTemperature;

    private:
        bool isUsingPWM;
};

#endif
