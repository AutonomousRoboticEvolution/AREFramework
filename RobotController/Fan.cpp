#include "Fan.hpp"

#include <iostream>
#include <bitset> // to display values in binary, e.g. std::cout << std::bitset<8>(myNumber) << std::endl
#include <unistd.h> // for usleep


Fan::Fan() {
    init(USE_PWM_OUTPUT);
}

void Fan::init(bool usePWM){
    wiringPiSetupGpio();
    if (usePWM){
            isUsingPWM = true;
            pinMode(FAN_GPIO_PIN, PWM_OUTPUT);
            std::cout << "using PWM for fan" << std::endl;
    }else{ // not using PWM (fan either on or off)
        isUsingPWM = false;
        pinMode (FAN_GPIO_PIN, OUTPUT);
        std::cout<<"not using PWM for fan"<<std::endl;
    }
};

void Fan::test() {
    std::cout << "turn on fan" << std::endl;
    turnOn();
    usleep(200000); // 2 seconds
    std::cout << "fan half speed" << std::endl;
    setPWMValue(500);
    usleep(200000); // 2 seconds
    std::cout << "turn off fan" << std::endl;
    turnOff();
}


void Fan::setTargetTemperature(float value){
    std::cout<<"set target temperature to: "<<value<<std::endl;
    targetTemperature = value;
}

void Fan::update(float currentTemperature){
//    std::cout<< "currentTemperature: "<< currentTemperature <<std::endl;
//    std::cout<< "targetTemperature: "<<targetTemperature <<std::endl;
    if (currentTemperature > targetTemperature){
        setPWMValue( K_OFFSET + K_P*( currentTemperature - targetTemperature) );
//        std::cout<< "computed fan speed: "<< K_OFFSET + K_P*( currentTemperature - targetTemperature) <<std::endl;
    }else{
        turnOff();
    }
}

void Fan::turnOn(){
    setPWMValue(FAN_PWM_MAX); // set to max speed
};

void Fan::turnOff(){
    setPWMValue(FAN_PWM_MIN);
};

void Fan::setPWMValue(int value){
    if ( isUsingPWM) {
        value = std::min(std::max(value, FAN_PWM_MIN), FAN_PWM_MAX); // ensure the value is within the PWM range
        pwmWrite (FAN_GPIO_PIN, value);
//        std::cout<<"fan set to "<<value<<std::endl;
    }else{
        if (value>0){
            digitalWrite (FAN_GPIO_PIN, HIGH);
        }else{
            digitalWrite (FAN_GPIO_PIN, LOW);
        }
    }
};

