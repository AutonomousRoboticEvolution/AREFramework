/**
	@file JointOrganTest.cpp
	@brief Test code for the JointOrgan class (based on Mike's MotorOrgan)
	@author Matt
*/

#include "I2CBus.hpp"
#include "JointOrgan.hpp"
#include "INA219_current_sensor.hpp"

// some stuff for timing the loop:
//#include "date.h"
#include <chrono>
#include <iostream>
#include <thread>

#define OUTPUT_TO_FILE
#ifdef OUTPUT_TO_FILE
#include <fstream>
#endif


#include <math.h>
#define PI 3.14159265

#define SHOULDER_ADDRESS 0x08
#define ELBOW_ADDRESS 0x07

// wiggle parameters:
//using namespace std::chrono
#define timestep 20ms // 100Hz
#define joint_update_divisor 50
#define period  4   // in multiples of timestep_us
#define phase_difference period/4
uint16_t i_step = 1;

// define the two joints:
JointOrgan shoulder(SHOULDER_ADDRESS);
JointOrgan elbow(ELBOW_ADDRESS);

// define the current sensor:
INA219_current_sensor INA(0x40);



// some global values
uint8_t measuredAngle = 0;

int sin_wave(int amplitude, int phase, int offset=0){
    float fraction = (float(i_step/joint_update_divisor)+phase)/period;
    return 128 + offset + amplitude * sin ( 2*PI*fraction );
}

int square_wave(int amplitude, int phase, int offset=0){
    float fraction = float(((i_step/joint_update_divisor)+phase)%period)/period;
    if (fraction>=0.5){
        return 128 + offset + amplitude;
    }else{
        return 126 + offset - amplitude;
    }
}

void run_wiggle(){

    // setup for loop timing:
    using namespace std::chrono;
//    using namespace date;
    auto next = steady_clock::now();
    auto prev = next - timestep;

    int elbow_value =128;
    int shoulder_value=128;

    #ifdef OUTPUT_TO_FILE
        steady_clock::time_point start_time = steady_clock::now();
        std::cout<< "t,shoulder demand,shoulder measured,elbow demand,elbow measured,current (mA),voltage (V)\n";
    #endif

    printf("starting");
    while (true)
    {
        i_step++;
//        printf("%i",i_step);

        if (i_step%joint_update_divisor == 0) { // update jiont values
            elbow_value = square_wave(50, 0, 25);
            shoulder_value = square_wave(50, phase_difference, 0);

            shoulder.setTargetAngle(shoulder_value);
            elbow.setTargetAngle(elbow_value);
//            printf("%i\t%i\n",shoulder_value,elbow_value);
        }

        // get power data:
//        float current_A = INA.read_current();
//        float voltage_V = INA.read_voltage(); // max value of 8000 is 32V


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
//        printf("===\n");
        // loop timing stuff:
        auto now = steady_clock::now();
        prev = now;
        next += timestep;
        std::this_thread::sleep_until(next);

    }

}


int command_line_testing(void) {

	//TEST Set joint angle from console
	int angle = 128;
	char c;
	printf("JOINT TEST\np increases angle\no decreases angle\nk turns off\nl keeps on but doens't move\nr reads angle\nx exits\n");
	do {
		c = getchar();
//		angle=(c - '0');
//		joint1.setTargetAngle(angle);
		switch (c) {
			case 'p' :
				angle=255;
                shoulder.setTargetAngle(angle);
				break;

			case 'o' :
				angle=0;
                shoulder.setTargetAngle(angle);
				break;
			case 'i' :
				angle=128;
                shoulder.setTargetAngle(angle);
				break;

			case 'k' :
                shoulder.setServoOff();
				break;

			case 'l' :
                shoulder.setServoOn();
				break;
            case 'r' :
                measuredAngle = shoulder.readMeasuredAngle();
                printf("Current measured angle: %02X\n", measuredAngle);
                break;
            case 't':
                printf("Test register: %02X\n", shoulder.readTestRegister()); // undocumented test register
		}
    printf("Target angle: %d/255\n", angle);
    getchar();



	} while (c != 'x');

	return 1;
}

int main(void) {
    //Initialise the bus
    I2CBus i2cBus1((char*)"/dev/i2c-1");
    i2cBus1.openBus();

    INA.init();

#ifdef OUTPUT_TO_FILE
    // direct any cout calls to file
    std::ofstream out("out.csv");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
#endif

    run_wiggle();
//    command_line_testing();
//        printf("Heatbeats: %#X, %#X\n", shoulder.readTestRegister(), elbow.readTestRegister());
//        printf("Heatbeats: %d, %d\n", shoulder.readTestRegister()==0x99, elbow.readTestRegister()==0x99);
//        printf("---\n");
}
