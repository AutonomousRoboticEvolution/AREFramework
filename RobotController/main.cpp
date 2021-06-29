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

//	daughterBoards.turnOff();

}


