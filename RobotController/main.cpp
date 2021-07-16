/**
	@file main.cpp
	@brief Test code only
	@author Mike Angus, Matt Hale
*/
#include <csignal>
#include <fstream>
#include <functional>
#include <memory>
#include <list>

#include "SensorOrgan.hpp"
#include "MotorOrgan.hpp"
#include "BrainOrgan.hpp"

#define WHEEL_ADDRESS 0x60
#define SENSOR_ADDRESS 0x30

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
    
    #define n_distances_to_test 100
    #define start_distance 200
    #define gap_between_distances start_distance
    #define n_repeats_per_distance 1

	/*
    //int listOfAddresses[] = {0x30,0x32,0x40,0x42,0x44};
    int listOfAddresses[] = {0x46};
    int n_sensors = 1;
    std::list<SensorOrgan> listOfSensors;
    std::cout<<"n_distances_to_test,"<<n_distances_to_test<<std::endl;
    std::cout<<"n_repeats_per_distance,"<<n_repeats_per_distance<<std::endl;
    std::cout<<"n_sensors,"<<n_sensors<<std::endl;
    std::cout<<"sensor addresses,";

    for (int i=0;i<n_sensors;i++){
        listOfSensors.push_back( SensorOrgan( listOfAddresses[i] ) ); // add a new wheel to the list, with the i2c address just extracted
        std::cout<<listOfAddresses[i]<<",";
    }
    std::cout<<std::endl;

    for (int i_distance=0;i_distance<n_distances_to_test;i_distance++){
        for (int i_repeat=0;i_repeat<n_repeats_per_distance;i_repeat++){
            int this_distance=start_distance + i_distance*gap_between_distances;
            for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
                //int raw_value = thisSensor->readTimeOfFlight();
                usleep(20000);
                //std::cout<<thisSensor->getOrganI2CAddress() <<","<< this_distance<<","<<thisSensor->readTimeOfFlight() <<std::endl;
                std::cout<<thisSensor->getOrganI2CAddress() <<","<<thisSensor->readTimeOfFlight() <<","<<thisSensor->readInfrared() <<std::endl;
            }
            usleep(20000);
        }
        std::cin.get();
    }
	*/
//	daughterBoards.turnOff();

}


