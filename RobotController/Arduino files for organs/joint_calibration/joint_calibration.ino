/*
	Author: Mike Angus
	Date: Sept 2021
	Description: Serial-based program for establishing end points of joint movements for calibration	
*/

#include <Servo.h>
#include <Wire.h>

#define DEBUG 1

#define SERVO_ENABLE 6 //change this to pin 7 for v1.2+ boards
#define SERVO_PWM 5
#define SERVO_POSITION_PIN A3
#define I2C_ENABLE 15

//CURRENT LIMITER DEFAULTS
#define DIG_POT_ADDRESS 0x2E //7-bit address of device. Wire library uses 7 bit addressing throughout
#define MIN_CURRENT_MA 330 //milliamps
#define MAX_CURRENT_MA 1250 //milliamps
#define DEFAULT_CURRENT_LIMIT MIN_CURRENT_MA

//Servo position
#define SERVO_POS_READING_MIN 30
#define SERVO_POS_READING_MAX 1000
#define SERVO_POS_READING_RANGE (SERVO_POS_READING_MAX - SERVO_POS_READING_MIN)


//Servo global variables
Servo joint_servo;

//Serial vars
char recvdChar;
String inputString;
int encoderReading;

void setup() {
	//Pin setup
	pinMode(SERVO_ENABLE, OUTPUT);
  pinMode(SERVO_PWM, OUTPUT);
  pinMode(I2C_ENABLE, OUTPUT);

  //Connect servo
  joint_servo.attach(SERVO_PWM);

  //Open serial port for debugging
  Serial.begin(115200);
  Serial.println("Joint Organ Reporting");

  //Set analog measurement to use external reference
  //MUST be called before any analogRead, to prevent hardware damage
  analogReference(EXTERNAL);

  //Set current limit to default value
  setCurrentLimit(DEFAULT_CURRENT_LIMIT/10);

  //Enable power to the servo
  servoEnable(true);

  //Enable I2C bus for current limiter
  Wire.begin();

  //Print instructions
  Serial.println("Type microsecond values to test. 550 and 2400 are reasonable starting points");
  Serial.println("Type 'e' to see the encoder reading at the current position");
}

void loop() {

while(Serial.available()) {
  //Read input in as a string
  recvdChar = Serial.read();

  //Special case: typing 'e' returns the encoder state
  if (recvdChar == 'e') {
    //Report the encoder reading at this position
    encoderReading = analogRead(SERVO_POSITION_PIN);
    Serial.print("Encoder reading at this position is: ");
    Serial.println(encoderReading);
  } else {
    //If not 'e', carry on producing a string
    inputString += recvdChar; //add next char to string
    delay(2); //slow to allow next character to enter buffer  
  }
}

if (inputString.length() > 0) {
  //Convert input string into a number
  int recvdValue = inputString.toInt();
  inputString = "";

  //Report the number entered and move the servo
  joint_servo.writeMicroseconds(recvdValue);
  Serial.print("Joint servo pulsewidth set to ");
  Serial.print(recvdValue);
  Serial.println(" us");
}

 // //Movement test code
//	int delayPeriod = 10;
//	for (int us_value = 400; us_value < 2500; us_value+= 10) {
//		joint_servo.writeMicroseconds(us_value);
//		delay(delayPeriod);
//	}
//	for (int us_value = 2500; us_value > 400; us_value-= 10) {
//		joint_servo.writeMicroseconds(us_value);
//		delay(delayPeriod);
//	}
//	
}


/***********Functions**************/
/*
  servoEnable
  @brief Enables/disables power to the servo via current limiter chip
  @param on_not_off TRUE to switch on, FALSE to switch off
*/
void servoEnable(bool on_not_off) {
  if (on_not_off) {
    digitalWrite(SERVO_ENABLE, HIGH);
  } else {
    digitalWrite(SERVO_ENABLE, LOW);
  }
}

/*
  extI2CEnable
  @brief Enables/disables external I2C bus connection via repeater chip
  @param on_not_off TRUE to switch on, FALSE to switch off
*/
void extI2CEnable(bool on_not_off) {
  if (on_not_off) {
    digitalWrite(I2C_ENABLE, LOW); //Active-low pin
  } else {
    digitalWrite(I2C_ENABLE, HIGH);
  }
}
/*
  setCurrentLimit
  @brief Sets the current limit for the joint servo.
  Note that this operation temporarily disconnects from external I2C bus.
  @param tens_of_milliamps Desired current limit in mA divided by 10. e.g. 33 = 330mA
*/
void setCurrentLimit (uint8_t tens_of_milliamps) {

  //Constrain to min and max current limits
  int current_limit_mA = tens_of_milliamps * 10;
  if (current_limit_mA > MAX_CURRENT_MA) {
    current_limit_mA = MAX_CURRENT_MA;
  } else if(current_limit_mA < MIN_CURRENT_MA) {
    current_limit_mA = MIN_CURRENT_MA;
  }

  //Calculate appropriate digital potentiometer setting
  //Current limit is 4500mA/Rtotal where R is in KOhms
  //Rtotal = 10K*(pot_value/127) + 3.6K
  //Rearranging gives pot_value = (127/10)*(4500/current_limit_mA - 3.6)
  uint8_t pot_value = 12.7*((float)4500/current_limit_mA - 3.6);

  //Write to digital potentiometer
  //Must isolate from external I2C bus first to not interact with other joints
  extI2CEnable(false);
  Wire.beginTransmission(DIG_POT_ADDRESS); // transmit to device
  Wire.write(pot_value);            // set register value
  int tx_info = Wire.endTransmission();     // stop transmitting 
  extI2CEnable(true);

  //Update global variable for current limit
//  current_limit_in_milliamps = current_limit_mA;

  //Debug info
  if (DEBUG) {
  Serial.print("[Current limiter setting] Wire transmission returned ");
  Serial.println(tx_info);
  Serial.print("Current limiter SETI resistor set to: ");
  Serial.print(3.6 + ((float)pot_value/127) * 10);
  Serial.println("K");
  Serial.print("Current limit set to: ");
  Serial.print(current_limit_mA);
  Serial.println("mA");
  }
}
