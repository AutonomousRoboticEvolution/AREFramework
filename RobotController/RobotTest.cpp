/**
	@file JointOrganTest.cpp
	@brief Test code for the JointOrgan class (based on Mike's MotorOrgan)
	@author Matt
*/

// choose what sort of test we are running:
//#define LEG_TEST
//#define TRICYCLE
#define MOTOR_ORGAN_TEST

#define OUTPUT_TO_FILE
#ifdef OUTPUT_TO_FILE
#include <fstream>
#endif

#ifdef MOTOR_ORGAN_TEST
#include "I2CBus.hpp"
#include "MotorOrgan.hpp"
#define MOTOR_ADDRESS 0x20
#endif


#include "I2CBus.hpp"
#include "JointOrgan.hpp"
#include "INA219_current_sensor.hpp"

// some stuff for timing the loop:
#include <chrono>
#include <iostream>
#include <thread>


#include <math.h>
#define PI 3.14159265

#define HORIZONTAL_JOINT_ADDRESS 0x08
#define VERTICAL_JOINT_ADDRESS 0x07
#define WHEEL_ADDRESS 0X67

// wiggle parameters:
#define timestep 20ms // 100Hz
#define joint_update_divisor 10
#define period  32   // in multiples of timestep_us*joint_update_divisor
#define phase_difference period/4
uint16_t i_step = 1;

// define the two joints:
#ifdef LEG_TEST
    JointOrgan shoulder(HORIZONTAL_JOINT_ADDRESS);
    JointOrgan elbow(VERTICAL_JOINT_ADDRESS);
#endif

#ifdef TRICYCLE
    #include "MotorOrgan.hpp"
    JointOrgan joint(HORIZONTAL_JOINT_ADDRESS);
    MotorOrgan wheel(WHEEL_ADDRESS);
#endif

// define the current sensor:
INA219_current_sensor INA(0x40);



// some global values
uint8_t measuredAngle = 0;

int sin_wave(int amplitude, int phase, int offset=0){
    float fraction = (float(i_step/joint_update_divisor)+phase)/period;
    return offset + amplitude * sin ( 2*PI*fraction );
}

int square_wave(int amplitude, int phase, int offset=0){
    float fraction = float(((i_step/joint_update_divisor)+phase)%period)/period;
    if (fraction>=0.5){
        return offset + amplitude;
    }else{
        return offset - amplitude;
    }
}

#ifdef LEG_TEST
void run_leg_wiggle(){

    // setup for loop timing:
    using namespace std::chrono;
    auto next = steady_clock::now();
    auto prev = next - timestep;

    int elbow_value =128;
    int shoulder_value=128;

    #ifdef OUTPUT_TO_FILE
        steady_clock::time_point start_time = steady_clock::now();
        std::cout<< "t,shoulder demand,shoulder measured,elbow demand,elbow measured,current (mA),voltage (V)\n";
    #endif

    printf("starting\n===\n");
    while (true)
    {
        i_step++;
//        printf("%i",i_step);

        if (i_step%joint_update_divisor == 0) { // update jiont values
            elbow_value = square_wave(50, 0, 25+128);
            shoulder_value = square_wave(50, phase_difference, 128);

            shoulder.setTargetAngle(shoulder_value);
            elbow.setTargetAngle(elbow_value);
//            printf("%i\t%i\n",shoulder_value,elbow_value);
        }

        // create output csv file
        #ifdef OUTPUT_TO_FILE
            steady_clock::time_point t2 = steady_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - start_time);
            std::cout
            << time_span.count() << ","
            << shoulder_value<<"," << shoulder.readMeasuredAngle() <<","
            << elbow_value<<"," << elbow.readMeasuredAngle() <<","
            << INA.read_current() <<","<<INA.read_voltage()<<"\n";
        #endif

        // loop timing stuff:
        auto now = steady_clock::now();
        prev = now;
        next += timestep;
        std::this_thread::sleep_until(next);
    }

}
#endif

#ifdef TRICYCLE
void run_tricycle(){
    // setup for loop timing:
    using namespace std::chrono;
    auto next = steady_clock::now();
    auto prev = next - timestep;

    int joint_value =128;
    int wheel_value=0;

    #ifdef OUTPUT_TO_FILE
        steady_clock::time_point start_time = steady_clock::now();
        std::cout<< "t,joint demand,joint measured,wheel demand,current (mA),voltage (V)\n";
    #endif

    printf("starting\n===\n");
    while (true)
    {
        i_step++;
        //print f("%i",i_step);

        if (i_step%joint_update_divisor == 0) { // update output values
            joint_value = square_wave(50, 0, 128);
            wheel_value = sin_wave(60,0,0);
            if (wheel_value<0){
                wheel_value=wheel_value*-1;
            }

//            joint.setTargetAngle(joint_value);
//            wheel.setSpeed(wheel_value);
            printf("Joint: %i\twheel: %i\n",joint_value,wheel_value);
        }

        printf("Current: %f, Voltage %f\n",INA.read_current(),INA.read_voltage());
        // create output csv file
        #ifdef OUTPUT_TO_FILE
            steady_clock::time_point t2 = steady_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - start_time);
            std::cout
                    << time_span.count() << ","
                    << joint_value<<"," << joint.readMeasuredAngle() <<","
                    << wheel_value<<","
                    << INA.read_current() <<","<<INA.read_voltage()<<"\n";
        #endif

        // loop timing stuff:
        auto now = steady_clock::now();
        prev = now;
        next += timestep;
        std::this_thread::sleep_until(next);
    }
}
#endif

#ifdef MOTOR_ORGAN_TEST
int motor_organ_tester(void) {

    //Initialise the bus
    I2CBus i2cBus1((char*)"/dev/i2c-1");
    i2cBus1.openBus();

    //Create a motor organ
    MotorOrgan motor1(MOTOR_ADDRESS);

    //TEST Set motor speed from console
    int8_t spd = 0;
    char c;
    printf("MOTOR TEST\np increases\no decreases\nk brakes\nc clears fault reg\nx exits\n");
    do {
        printf("Speed: %d\n", spd);
        c = getchar();
        switch (c) {
            case 'p' :
                spd+=20;
                motor1.setSpeed(spd);
                break;

            case 'o' :
                spd-=20;
                motor1.setSpeed(spd);
                break;

            case 'k' :
                spd = 0;
                motor1.brake();
                //motor1.setSpeed(spd);
                break;
            case 'c' :
                motor1.clearFaultReg();
                printf("FAULTREG: %02X\n", motor1.readFaultReg());
                break;
        }
        //printf("FAULTREG: %02X\n", motor1.readFaultReg());
    } while (c != 'x');

    return 1;
}
#endif


int main(void) {
    //Initialise the bus
    I2CBus i2cBus1((char *) "/dev/i2c-1");
    i2cBus1.openBus();

    INA.init();

#ifdef OUTPUT_TO_FILE
    // direct any cout calls to file
    std::ofstream out("out.csv");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
#endif

#ifdef LEG_TEST
    run_leg_wiggle();
#endif

#ifdef TRICYCLE
    run_tricycle();
#endif

#ifdef MOTOR_ORGAN_TEST
    motor_organ_tester();
#endif

}