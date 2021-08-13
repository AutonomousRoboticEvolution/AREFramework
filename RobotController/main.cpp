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

#include "Organ.hpp"
#include "SensorOrgan.hpp"
#include "MotorOrgan.hpp"
#include "BrainOrgan.hpp"
#include "JointOrgan.hpp"

#define DO_JOINT_TORQUE_TEST false

#define DO_JOINT_TEST false
#define JOINT1_ADDRESS 0x09
#define JOINT2_ADDRESS 0x08
#define JOINT1_CURRENT_LIMIT 66 //x10 = 660mA
#define JOINT2_CURRENT_LIMIT 33 //x10 = 330mA


#define DO_WHEEL_TEST true
#define WHEEL_ADDRESS 0x65

#define DO_SENSOR_TEST false


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
    //ledDriver.flash();


/************ program for testing the torque of the joint - just move back and forth, having set the current limit ********************************************/
if (DO_JOINT_TORQUE_TEST){
    
    int jointAddress = 0x09;

    #define START_CURRENT 1000
    #define CURRENT_STEP 50

    std::cout<<"Doing the joint torque test for "<< jointAddress<<std::endl;
    
    // make joint
    JointOrgan joint(jointAddress);
    joint.setTargetAngleNormalised(1);
    sleep(2);
    
    int currentLimit_mA = START_CURRENT-CURRENT_STEP;
    bool has_failed = true;
    while(has_failed){
        //Set new current limit
        currentLimit_mA+=CURRENT_STEP;
        std::cout<<"Current limit: "<<currentLimit_mA<<"mA"<<std::endl;
        joint.setCurrentLimit(currentLimit_mA/10);
        sleep(1);

        joint.setTargetAngleNormalised(-1);
        sleep(2);
        int measured_value = joint.readMeasuredAngle();
        if (measured_value<-70){
            has_failed=false;
            std::cout<<"Success!"<<std::endl;
            joint.setTargetAngleNormalised(1);
        }else{
            std::cout<<"Fail..."<<measured_value<<std::endl;
            if(currentLimit_mA>=1200){
                std::cout<<"Max torque, still failed"<<std::endl;
                has_failed=false;
            }
            joint.setTargetAngleNormalised(1);
            sleep(4);
        }
    }
}

/************ joint test ********************************************/
if (DO_JOINT_TEST){

const int stepDuration =  10000; //us
const int numRepetitions = 10;
const int proximalMin = 80;
const int proximalMax = 135;
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

//Set to start position
proximalJoint.setTargetAngle(proximalMin);
distalJoint.setTargetAngle(distalMin);
sleep(3);

//Walking motion
for (int n=0; n<numRepetitions; ++n) {
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
}

}


/************ Wheel test ********************************************/
if (DO_WHEEL_TEST){
    daughterBoards.turnOn(RIGHT);
    std::cout<<"Testing wheel functionality"<<std::endl;
    MotorOrgan myWheel(WHEEL_ADDRESS);

    if(myWheel.testConnection()){
        std::cout<<"wheel connection successful"<<std::endl;
        ledDriver.flash(GREEN);
        
        for (int current_limit_mA = 200; current_limit_mA<=800; current_limit_mA+=200){
            myWheel.setCurrentLimit(current_limit_mA/10);
            sleep(1);
            myWheel.setSpeed(20);
            sleep(3);
            int speed=0;
            for(int i=0;i<10;i++){
                std::cout<<i<<std::endl;
                speed+=myWheel.readMeasuredVelocity();
                usleep(20000);
            }
            std::cout<<"Current limit: "<<current_limit_mA<<"mA, measured veloicty: "<<float(speed)/10<<" ticks/timestep"<<std::endl;
            myWheel.standby();
            sleep(1);
        }

    }else{
        std::cout<<"wheel connection failed"<<std::endl;
        ledDriver.flash(RED);
    }
    
    myWheel.standby();
    //daughterBoards.turnOff();
    ledDriver.flash(BLUE);
}

/************ Sensor test ********************************************/
if(DO_SENSOR_TEST){

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
        if(thisSensor->testConnection()){
            organDaugherBoardLocations[i]=LEFT;
            std::cout<<"\tleft"<<std::endl;
        }else{
            daughterBoards.turnOn(RIGHT);
            if(thisSensor->testConnection()){
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
	

}


