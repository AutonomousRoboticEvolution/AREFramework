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
#include <cassert>

#include "SensorOrgan.hpp"
#include "MotorOrgan.hpp"
#include "BrainOrgan.hpp"
#include "JointOrgan.hpp"

#define WHEEL_ADDRESS 0x60
#define JOINT1_ADDRESS 0x09
#define JOINT2_ADDRESS 0x08
#define JOINT1_CURRENT_LIMIT 66 //x10 = 660mA
#define JOINT2_CURRENT_LIMIT 33 //x10 = 330mA


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
//    batteryMonitor.testingDisplayBatteryLevels();
//    batteryMonitor.printAllPages();

/************ Fan and daughter boards enable testing *********************************/
    Fan fan;
    DaughterBoards daughterBoards;
    //fan.test();
    // daughterBoards.test();

/************ LEDs ********************************************/
    LedDriver ledDriver(0x6A); // <- the Led driver is always the same i2c address, it cannot be cahnged
    ledDriver.init();
    ledDriver.flash();

/************ joint test ********************************************/
bool do_joint_test = false;
if (do_joint_test){
    uint8_t address = 0x08;
    JointOrgan myJoint(address);
    myJoint.testFunction();
}

const int stepDuration =  10000; //us
const int numRepetitions = 10;
const int proximalMin = 60;
const int proximalMax = 120;
const int distalMin = 30;
const int distalMax = 150;

//Create 2 joints
JointOrgan proximalJoint(JOINT1_ADDRESS); //proximal
JointOrgan distalJoint(JOINT2_ADDRESS); //distal

//Set current limits
proximalJoint.setCurrentLimit(JOINT1_CURRENT_LIMIT);
sleep(1);
distalJoint.setCurrentLimit(JOINT2_CURRENT_LIMIT);
sleep(1);

//Set to centre position
proximalJoint.setTargetAngle(90);
distalJoint.setTargetAngle(90);
sleep(3);

//Distal forward
for (int i=distalMin; i<=distalMax; ++i) {
    distalJoint.setTargetAngle(i);
    usleep(stepDuration);
}

//Proximal down
for (int i=proximalMin; i<=proximalMax; ++i) {
    proximalJoint.setTargetAngle(i);
    usleep(stepDuration);
}

//Distal back
for (int i=distalMax; i>=distalMin; --i) {
    distalJoint.setTargetAngle(i);
    usleep(stepDuration);
}

//Proximal up
for (int i=proximalMax; i>=proximalMin; --i) {
    proximalJoint.setTargetAngle(i);
    usleep(stepDuration);
}

//Wiggle
// for (int n=0; n<numRepetitions; ++n) {
//     for (int i=0; i<=180; ++i) {
//         proximalJoint.setTargetAngle(i);
//         distalJoint.setTargetAngle(i);
//         usleep(stepDuration);
//     }
//     for (int i=180; i>=0; --i) {
//         proximalJoint.setTargetAngle(i);
//         distalJoint.setTargetAngle(i);
//         usleep(stepDuration);
//     }
// }

/************ Wheel test ********************************************/
//    MotorOrgan myWheel(WHEEL_ADDRESS);
//	myWheel.test();

/************ Sensor test ********************************************/

    /*

    std::list<SensorOrgan> listOfSensors;
    listOfSensors.push_back( SensorOrgan( 0x30 ) );
    listOfSensors.push_back( SensorOrgan( 0x32 ) );
    listOfSensors.push_back( SensorOrgan( 0x34 ) );
    listOfSensors.push_back( SensorOrgan( 0x36 ) );
    listOfSensors.push_back( SensorOrgan( 0x38 ) );
    listOfSensors.push_back( SensorOrgan( 0x3A ) );
    listOfSensors.push_back( SensorOrgan( 0x3C ) );
    listOfSensors.push_back( SensorOrgan( 0x3E ) );
    boardSelection organDaugherBoardLocations[listOfSensors.size()];
    
    int i=-1;
    std::cout<<"Finding which daughter board each organ is attached to:"<<std::endl;
    for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
        i++;
        daughterBoards.turnOn(LEFT);
        if(thisSensor->test()){
            organDaugherBoardLocations[i]=LEFT;
            std::cout<<"\tleft"<<std::endl;
        }else{
            daughterBoards.turnOn(RIGHT);
            if(thisSensor->test()){
            organDaugherBoardLocations[i]=RIGHT;
                std::cout<<"\tright"<<std::endl;
            }else{
            organDaugherBoardLocations[i]=NONE;
                std::cout<<"\tmissing!"<<std::endl;
            }
        }
    }
    
    int distance=0;
    std::cout<<std::endl;
    while(distance<1500){
        distance += 100;
        i=-1;
        std::cout<<distance<<",";
        for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
            i++;
            usleep(5000);
            daughterBoards.turnOn(organDaugherBoardLocations[i]);
            usleep(5000);
            //std::cout<<thisSensor->readTimeOfFlight()<<",";
            std::cout<<thisSensor->readInfrared()<<",";
        }
        //std::cout<<std::endl;
        std::cin.get();
    }
    
    
    /*
    #define n_distances_to_test 15
    #define start_distance 100
    #define gap_between_distances start_distance
    #define n_repeats_per_distance 50
    
    SensorOrgan mySensor(0x36);
    daughterBoards.turnOn(LEFT);

    std::cout<<std::endl;

    for (int i_distance=0;i_distance<n_distances_to_test;i_distance++){
        int this_distance=start_distance + i_distance*gap_between_distances;
        std::cout<<this_distance<<",";
        for (int i_repeat=0;i_repeat<n_repeats_per_distance;i_repeat++){
            std::cout<<mySensor.readInfrared()<<",";
            usleep(20000);
        }
        std::cin.get();
    }
    */

	

}


