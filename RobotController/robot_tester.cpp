#include "Camera.hpp"
#include "Fan.hpp"
#include "DaughterBoards.hpp"
#include "BatteryMonitor.hpp"
#include "LedDriver.hpp"
#include "MotorOrgan.hpp"
#include "SensorOrgan.hpp"
#include "JointOrgan.hpp"
#include <iostream>

#include <wiringPi.h> //for timing functions


void jointTest(){
    JointOrgan j1(0x0B);
    JointOrgan j2(0x0C);

    if (j1.testConnection()){std::cout<<"Joint 1 good"<<std::endl;} else {std::cout<<"Joint 1 bad!"<<std::endl;}
    if (j2.testConnection()){std::cout<<"Joint 2 good"<<std::endl;} else {std::cout<<"Joint 2 bad!"<<std::endl;}

#define period 5000 // milliseconds
#define number_of_periods 5
#define amplitude1 0.8 // 0-1
#define amplitude2 0.6 // 0-1
#define phase_offset period/4 // milliseconds
#define pi 3.14159265359
#define timestep 10 // milliseconds
#define N_timesteps (number_of_periods*period)/timestep

    unsigned int start_time = millis();

    j1.setCurrentLimit(33);
    j2.setCurrentLimit(33);

    // go to start positions
    j1.setTargetAngleNormalised(0);
    j2.setTargetAngleNormalised( 2*pi*phase_offset/period);

    delay(2); // allow time to move to start positions

    for (unsigned int i_time=0; i_time<N_timesteps; i_time++){
        while(millis()< start_time + i_time*timestep){}
        //std::cout<< "time: "<< float(i_time*timestep)/1000.0 <<"s" <<std::endl;
        j1.setTargetAngleNormalised(amplitude1*sin(2*pi*(i_time*timestep)/period));
        j2.setTargetAngleNormalised(amplitude2*sin( 2*pi*((i_time*timestep)+phase_offset)/period));
    }

}

void cameraTest(){
    #define basic false

    Camera cam;
    cam.setTagsToLookFor({10});

    if(basic){
        cam.presenceDetect("debugOutput"); // <- with saving the image to /home/pi/debugOutput.png
    }else{
        MotorOrgan w1(0x66);
        MotorOrgan w2(0x61);

        int n=0;
        for (float i=0.2; i<=1.0; i+=0.025){
            n++;
            // start moving
            w1.setSpeedNormalised(i);
            w2.setSpeedNormalised(i);

            //wait for movement to settle
            delayMicroseconds(500000);
            cam.presenceDetect("debugOutput"+std::to_string(n)); // <- with saving the image to /home/pi/debugOutput.png

        }
        w1.standby();
        w2.standby();
    }
}

void wheelTest(DaughterBoards daughterBoards, LedDriver ledDriver){
    daughterBoards.turnOn(RIGHT);
    std::cout<<"Testing wheel functionality"<<std::endl;
    MotorOrgan myWheel(0x66);


    myWheel.standby();
    daughterBoards.turnOff();
    ledDriver.flash(GREEN);
}


int main(int argc, char** argv){

    std::cout<<"Testing robot"<<std::endl;

    /*** Battery monitor testing: ***/
    BatteryMonitor batteryMonitor;
    //    batteryMonitor.setBatteryChargeRemaining(2000);
    batteryMonitor.testingDisplayBatteryLevels();
    //    batteryMonitor.printAllPages();


    // setup daughter boards
    DaughterBoards db;
    db.init();
    db.turnOn(LEFT);

    // Fan (test)
    //Fan fan;
    //fan.test();

    /*** LEDs ***/
    LedDriver ledDriver(0x6A); // <- the Led driver is always the same i2c address, it cannot be cahnged
    ledDriver.init();
    ledDriver.flash(BLUE, 1000000 ,20);

    /*** camera blur test ***/
    //cameraTest();

    /*** joint test ***/
    jointTest();


}
