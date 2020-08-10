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
    imu.init();

 //    int err = 0;
 //   	err = wiringPiSPISetup(0,100000);
 //   	uint8_t spi[64] = {0};

 //   	//Disable I2C
 //   	spi[0] = MPUREG_USER_CTRL;
 //   	spi[1] = BIT_I2C_IF_DIS;
 //   	wiringPiSPIDataRW(0, spi, 2);
 //   	printf("After i2c disable Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);

 //   	//Reset
 //   	spi[0] = MPUREG_PWR_MGMT_1;
 //   	spi[1] = BIT_H_RESET;
 //   	wiringPiSPIDataRW(0, spi, 2);
 //   	usleep(100000);
 //   	spi[0] = MPUREG_SIGNAL_PATH_RESET;
 //   	spi[1] = 0x7;	//00000111, gyro, accel and temp reset
 //   	wiringPiSPIDataRW(0, spi, 2);
 //   	usleep(100000);
   	
 //   	printf("After reset Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);


 //   	//Wake up and set to use gyroz clock
 //   	spi[0] = MPUREG_PWR_MGMT_1;
 //   	spi[1] = MPU_CLK_SEL_PLLGYROZ;
 //   	wiringPiSPIDataRW(0, spi, 2);
 //   	imu.write8To(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
 //   	usleep(150000);
 //   	printf("After gyroz clock set Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);

	// //Disable I2C (again!?)
 // 	spi[0] = MPUREG_USER_CTRL;
 // 	spi[1] = BIT_I2C_IF_DIS;
 // 	wiringPiSPIDataRW(0, spi, 2);
 // 	printf("After 2nd i2c disable Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);

 	

 // 	//Set sample rate
 // 	spi[0] = MPUREG_SMPLRT_DIV;
 // 	spi[1] = 1;
 // 	wiringPiSPIDataRW(0, spi, 2);
 // 	printf("After smplrt set Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);

 // 	//Set filtering
 // 	spi[0] = MPUREG_CONFIG;
 // 	spi[1] = BITS_DLPF_CFG_5HZ;
 // 	wiringPiSPIDataRW(0, spi, 2);
 // 	printf("After config set Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);

 // 	// //Disable interrupts
 // 	spi[0] = MPUREG_INT_ENABLE;
 // 	spi[1] = 0x00;
 // 	wiringPiSPIDataRW(0, spi, 2);
 // 	printf("After interrupt disable Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);

 //  //Check whoami
 //  spi[0] = MPUREG_WHOAMI | READ_FLAG; //Whoami register
 //  spi[1] = 0x00;  //nowt, just clock out the contents of whoami
 //  wiringPiSPIDataRW(0, spi, 2);
 //  printf("After whoami Spi: [0]:%0X , [1]:%0X, err: %d\n", spi[0], spi[1], err);

 //  uint8_t whoamival = imu.read8From(MPUREG_WHOAMI);
 //  printf("Whoami returned %0X\n", whoamival);

 // 	//Try reading loads of registers
 // 	//Don't forget the read flag!
 // 	//---doesn't look like read flag gets applied to subsequent registers if reading a block of many?
 // 	//Output is shifted by one [0]->[1]
 // 	spi[0] = MPUREG_PWR_MGMT_1 | READ_FLAG;
 // 	wiringPiSPIDataRW(0, spi, 2);
 // 	for (int i=0; i<3; ++i) {
 // 		printf("Spi[%0X] : %0X\n",i+MPUREG_PWR_MGMT_1-1,spi[i]);
 // 	}

  for(int i=0; i<200; ++i) {
    int16_t data = imu.read16From(MPUREG_ACCEL_XOUT_H);
    printf("Data: %d %0.1f %0X\n", data, (float)data/16384, data);
    usleep(10000);
  }

  int16_t temp = imu.read16From(MPUREG_TEMP_OUT_H);
  printf("Temperature: %.1f C (%0X hex)\n", (float)temp/340 + 36.53, temp);

  // //8bit version
  // for(int i=0; i<200; ++i) {
  //   uint8_t datah = imu.read8From(MPUREG_ACCEL_XOUT_H);
  //   uint8_t datal = imu.read8From(MPUREG_ACCEL_XOUT_L);
  //   int16_t data = datah << 8 | datal;
  //   printf("Data: %0.1f %0X (%0X %0X)\n", (float)data, data, datah, datal);
  //   usleep(10000);
  // }
 	

	/***************************************************************************/


    // ledDriver.testWrite(0x00, 0x01);	//Mode1 bit [4] clear to enable osc
    // ledDriver.testWrite(0x01, 0x28);	//Mode2 bit [3] set output change on ack; [5] global blink
    // ledDriver.testWrite(0x14, 0xFF);	//LEDOUT0 to 11111111 pwmable and blinkable
    // ledDriver.testWrite(0x15, 0xFF);	//LEDOUT1 to 11111111 pwmable and blinkable
    // ledDriver.testWrite(0x16, 0xFF);	//LEDOUT2 to 11111111 pwmable and blinkable
    // ledDriver.testWrite(0x17, 0xFF);	//LEDOUT3 to 11111111 pwmable and blinkable
    // ledDriver.testWrite(0x02, 0x00);	//Half beans to first led (green)
    // ledDriver.testWrite(0x03, 0x8F);	//3/4 beans to 2nd led (red)
    // ledDriver.testWrite(0x04, 0x0F);	//Half beans to 3rd led (blue)
    // ledDriver.testWrite(0x13, 0x0A);	//GRPFREQ to ~0.5s(C)
    // ledDriver.testWrite(0x12, 0x5F);	//GRPPWM duty cycle to 0.5

    // printf("Mode1: %0X\n", ledDriver.testRead(0x00));
    // printf("Mode2: %0X\n", ledDriver.testRead(0x01));
    // printf("LEDOUT0: %0X\n", ledDriver.testRead(0x14));
    // printf("PWM0: %0X\n", ledDriver.testRead(0x2));
    // printf("PWM1: %0X\n", ledDriver.testRead(0x3));
    // printf("PWM2: %0X\n", ledDriver.testRead(0x4));
    // printf("PWM3: %0X\n", ledDriver.testRead(0x5));

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