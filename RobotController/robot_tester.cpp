#include "Camera.hpp"
#include "Fan.hpp"
#include "DaughterBoards.hpp"
#include "BatteryMonitor.hpp"
#include "LedDriver.hpp"
#include "MotorOrgan.hpp"
#include "SensorOrgan.hpp"
#include <iostream>

#include <wiringPi.h> //for timing functions



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
    cameraTest();

}
