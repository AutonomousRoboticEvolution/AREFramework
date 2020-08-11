/**
	@file MotorOrganTest.cpp
	@brief Test code for the MotorOrgan class.
	Allows control of motor and shows how the bus should be set up.
	@author Mike Angus
*/
#include <csignal>
#include <fstream>
#include <functional>
#include <memory>

#include "SensorOrgan.hpp"
#include "MotorOrgan.hpp"
#include "BrainOrgan.hpp"
// #include <wiringPiSPI.h>
#include "IMU.hpp"
//#include "../Cplusplus_Evolution/ERFiles/control/FixedStructreANN.h"

#define SENSOR1 0x72
#define SENSOR2 0x73
#define MOTOR1 0x68
#define MOTOR2 0x66

#define SCALE_MOTOR1 6.0
#define SCALE_MOTOR2 6.0

#define LED_DRIVER_ADDR 0x6A


// void split_line(std::string& line, char delim, std::vector<std::string>& values)
// {
//     size_t pos = 0;
//     while ((pos = line.find(delim, (pos + 0))) != string::npos) {
//         string p = line.substr(0, pos);
//         values.push_back(p);
//         line = line.substr(pos + 1);
//     }
//     while ((pos = line.find(delim, (pos + 1))) != string::npos) {
//         string p = line.substr(0, pos);
//         values.push_back(p);
//         line = line.substr(pos + 1);
//     }

//     if (!line.empty()) {
//         values.push_back(line);
//     }
// }

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

// std::vector<std::string> load_params_list(const std::string &filename)
// {
//     std::vector<std::string> brain_params;
//     //load brain params from file
//     std::ifstream genome_file(filename);
//     if (!genome_file) {
//         std::cerr << "Could not load " << filename << std::endl;
//         std::exit(1);
//     }

//     {
//         std::string value;
//         while (std::getline(genome_file, value, ',')) {
//             if (value.find('\n') != string::npos) {
//                 split_line(value, '\n', brain_params);
//             } else {
//                 brain_params.push_back(value);
//             }
//         }
//     }

//     return brain_params;
// }

int main()
{
    setup_sigint_catch();

    //Create and test led driver
    // LedDriver ledDriver(LED_DRIVER_ADDR);
    // ledDriver.test();
    
    //SPI test code for mpu6000
    IMU imu;
    imu.test(50, 100000);

    // Settings settings;

 //    FixedStructureANN controller;
 //    controller.settings = std::make_shared<Settings>();
 //    controller.randomNum = std::make_shared<RandNum>(0);
 //    controller.init(2, 0, 2);
 //    std::vector<std::string> brain_params = load_params_list("genome.csv");
 //    controller.setControlParams(brain_params);

	// // Variables
	// int8_t speed1 = 0;
	// int8_t speed2 = 0;
	// float reading1 = 0;
	// float reading2 = 0;
	// //Create a sensor and motor  organs
	// SensorOrgan sensor1(SENSOR1);
	// SensorOrgan sensor2(SENSOR2);
	// MotorOrgan motor1(MOTOR1);
	// MotorOrgan motor2(MOTOR2);
	// // Initialize motors
	// motor1.brake();
	// motor2.brake();
	// sleep(1); // Give chance to stop motor at start of code
	// // Initialize ADC sensor
	// //sensor1.ADCinit();
	// //sensor2.ADCinit();
	// sensor1.initProximity();
	// sensor2.initProximity();
	// // Infinite loop
	// do {
	// 	// Take readings from sensors
	// 	reading1 = sensor1.calibratedProximityReading();
	// 	reading2 = sensor2.calibratedProximityReading();

	// 	std::vector<float> inputs = { reading1, reading2 };
	// 	std::vector<float> output = controller.update(inputs);

	// 	speed1 = output[0] * 63.0 * SCALE_MOTOR1;
	// 	speed2 = output[1] * 63.0 * SCALE_MOTOR2;

	// 	motor1.setSpeed(speed1);
	// 	motor2.setSpeed(speed2);

	// 	//Loop timer
	// 	usleep(50*1000); // 50ms -> 20Hz
	// } while (running);

	// motor1.brake();
	// motor2.brake();
	// return 0;
}