/**
	@file main.cpp
	@brief Test code only
	@author Mike Angus, Matt Hale
*/
#include <csignal>
#include <fstream>
#include <functional>
#include <memory>

#include "SensorOrgan.hpp"
#include "MotorOrgan.hpp"
#include "BrainOrgan.hpp"

#define WHEEL_ADDRESS 0x60
#define SENSOR_ADDRESS 0x32

#define LED_DRIVER_ADDR 0x6A

bool running = true;

void sigint_handler(int s)
{
    running = false;
}

void setup_sigint_catch()
{
    struct sigaction sigIntHandler {};
    memset(&sigIntHandler, 0, sizeof(sigIntHandler));

    sigIntHandler.sa_handler = sigint_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);

}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include <bitset> // for displaying binary values


int main()
{

/************ Battery monitor testing ********************************************/
    BatteryMonitor batteryMonitor;
//    batteryMonitor.setBatteryChargeRemaining(2000);
    batteryMonitor.testingDisplayBatteryLevels();
//    batteryMonitor.printAllPages();

/************ Fan and daughter boards enable testing *********************************/
    Fan fan;
    DaughterBoards daughterBoards;
    //fan.test();
    //daughterBoards.test();

/************ Wheel test ********************************************/
    MotorOrgan myWheel(WHEEL_ADDRESS);
//	myWheel.test();


/************ Sensor test ********************************************/
    SensorOrgan mySensor(SENSOR_ADDRESS);
	mySensor.test();
    
    int listOfAddresses[] = {0x30,0x32,0x40,0x42,0x44};
    int n_sensors = 5;
    for (int i=0,i<n_sensors,i++){
        listOfSensors.push_back( SensorOrgan( listOfAddresses[i] ) ); // add a new wheel to the list, with the i2c address just extracted
    }
    
    for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
        int raw_value = thisSensor->readTimeOfFlight();
        sensor_vals.push_back( std::max ( std::min(1.0,float(raw_value)/1000.0) , 0.0) );
    }
//	daughterBoards.turnOff();

}


