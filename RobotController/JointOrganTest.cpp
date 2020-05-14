/**
	@file JointOrganTest.cpp
	@brief Test code for the JointOrgan class (based on Mike's MotorOrgan)
	@author Matt
*/

#include "I2CBus.hpp"
#include "JointOrgan.hpp"

#include <math.h>
#define PI 3.14159265

#define JOINT1_ADDRESS 0x08
#define JOINT2_ADDRESS 0x07

// wiggle parameters:
#define timestep_us 100000 // 10Hz
#define period  32// in multiples of timestep_us
#define phase_difference period/4
uint16_t i_step = 1;

// define the two joints:
JointOrgan shoulder(JOINT1_ADDRESS);
JointOrgan elbow(JOINT2_ADDRESS);

// some global values
uint8_t measuredAngle = 0;

int sin_wave(int amplitude, int phase, int offset=0){
    float fraction = (float(i_step)+phase)/period;
    return 128 + offset + amplitude * sin ( 2*PI*fraction );
}

void run_wiggle(){


    while(true) {
        usleep(timestep_us);
        i_step++;

        int  elbow_value = sin_wave(0,0,0);
        int shoulder_value = sin_wave(50,phase_difference,50);

//        printf("%d, %d\n", shoulder_value , elbow_value);
//        printf("Heartbeats: %X, %X\n", shoulder.readTestRegister(), elbow.readTestRegister());
        printf("measured: %d, %d\n", shoulder.readMeasuredAngle(), elbow.readMeasuredAngle());
        shoulder.setTargetAngle(shoulder_value);
        elbow.setTargetAngle(elbow_value);
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


	run_wiggle();
//    command_line_testing();
//    while(true) {
//        usleep(1000000);
//        uint8_t shoulder_reading = shoulder.readMeasuredAngle();
//        uint8_t elbow_reading = elbow.readMeasuredAngle();
//
//        printf("Shoulder: %d, Elbow: %d\n", shoulder_reading, elbow_reading);
//
//        printf("Heatbeats: %#X, %#X\n", shoulder.readTestRegister(), elbow.readTestRegister());
//        printf("Heatbeats: %d, %d\n", shoulder.readTestRegister()==0x99, elbow.readTestRegister()==0x99);
//        printf("---\n");
//    }
}
