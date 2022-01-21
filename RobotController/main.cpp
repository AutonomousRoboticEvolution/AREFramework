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
#include "Camera.hpp"

#define DO_JOINT_TORQUE_TEST false

#define DO_JOINT_TEST false
#define JOINT1_ADDRESS 0x09
#define JOINT2_ADDRESS 0x08
#define JOINT1_CURRENT_LIMIT 66 //x10 = 660mA
#define JOINT2_CURRENT_LIMIT 33 //x10 = 330mA


#define DO_WHEEL_TEST false
#define WHEEL_ADDRESS 0x65

#define DO_SENSOR_TEST false
#define DO_MULTI_SENSOR_TEST false


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

void turtle_wiggle(DaughterBoards daughterBoards){

    daughterBoards.turnOn(RIGHT);
    JointOrgan left_shoudler(0x08);
    JointOrgan left_elbow(0x09);
    JointOrgan right_shoudler(0x0a);
    JointOrgan right_elbow(0x0b);

    std::cout<<"doing test connections..."<<std::endl;
    std::cout<<"0x08: "<<left_shoudler.testConnection()<<std::endl;
    std::cout<<"0x09: "<<left_elbow.testConnection()<<std::endl;

    delay(1);
    left_shoudler.setTargetAngleNormalised(0.5);
    left_elbow.setTargetAngleNormalised(0.5);
    right_shoudler.setTargetAngleNormalised(0.5);
    right_elbow.setTargetAngleNormalised(0.5);

    delay(1);
    left_shoudler.setTargetAngleNormalised(0.2);
    left_elbow.setTargetAngleNormalised(0.5);
    right_shoudler.setTargetAngleNormalised(0.2);
    right_elbow.setTargetAngleNormalised(0.5);

    delay(1);
    left_shoudler.setTargetAngleNormalised(0.2);
    left_elbow.setTargetAngleNormalised(0.5);
    right_shoudler.setTargetAngleNormalised(0.2);
    right_elbow.setTargetAngleNormalised(0.5);

}

bool wheel_torque_test_helper(MotorOrgan wheel,int value){
    std::cout<< "Limit: "<<value*10<<"mA";

    wheel.setCurrentLimit(value);
    sleep(1);
    wheel.setSpeed(20);
    sleep(2);
    
    int speed=0;
    int current=0;
    for(int i=0;i<10;i++){
        speed+= wheel.readMeasuredVelocity();
        current+= wheel.readMeasuredCurrent();
        usleep(20000);
    }
    wheel.standby();
    std::cout<<", speed: "<<float(speed)/10.0 <<", current: "<<float(current)/10.0 <<std::endl;
    if (speed>15){return true;}
    else {return false;}
}

void wheel_torque_test(MotorOrgan wheel){
    
    int lower_bound=0;
    int upper_bound=100;
    bool found_value=false;

    while(found_value==false){
        
        int new_test_value = lower_bound + (upper_bound-lower_bound)/2;
        if ( wheel_torque_test_helper(wheel, new_test_value) ){
            // passed
            upper_bound=new_test_value;
        }else{
            // failed
            lower_bound=new_test_value;
        }
        
        if (upper_bound-lower_bound<=1) { found_value=true; }
    }

    std::cout<<"required torque is between "<<lower_bound*10<<" and "<<upper_bound*10<<" mA.";
    

}

void cameraTest(){
    Camera cam;
    cam.setTagsToLookFor({14});
    cam.presenceDetect();
}

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
    // daughterBoards.test();

/************ LEDs ********************************************/
    LedDriver ledDriver(0x6A); // <- the Led driver is always the same i2c address, it cannot be cahnged
    ledDriver.init();
    ledDriver.flash(BLUE, 1000000 ,20);

    cameraTest();

    turtle_wiggle(daughterBoards);

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
    
    
    wheel_torque_test(myWheel);
    
    myWheel.standby();
    //daughterBoards.turnOff();
    ledDriver.flash(BLUE);
}

/************ Sensor test ********************************************/
/************ Sensor test ********************************************/
if (DO_SENSOR_TEST){

    SensorOrgan mySensor(0x30);
    daughterBoards.turnOn(LEFT);

    std::cout<<std::endl;
    
    while(1){
        std::cin.get();
        int n_successes = 0;
        /*for (int i=0;i<100;i++){
            if (mySensor.readTimeOfFlight() < 1500){
                n_successes++;
            }
            usleep(50000);
        }
        std::cout<<float(n_successes)<<"%"<<std::endl;
        */
        std::cout<<mySensor.readInfrared()<<std::endl;
    }
}

if(DO_MULTI_SENSOR_TEST){

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

    

}
	

}


