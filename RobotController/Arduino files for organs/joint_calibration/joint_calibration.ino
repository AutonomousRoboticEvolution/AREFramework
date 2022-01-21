/*
	Author: Mike Angus
	Date: Sept 2021
	Description: Serial-based program for establishing end points of joint movements for calibration	
*/

#include <Servo.h>
#include <Wire.h>

#define DEBUG 0

#define SERVO_ENABLE 7 //change this to pin 6 for v1.1 boards
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

//Calibration params
#define PWM_START_LO_US 600
#define PWM_START_HI_US 2350
#define TEST_INCREMENT_US 10
#define MANUAL_INCREMENT_US 10
#define MIN_MEASURED_ENCODER_DIFFERENCE 2 //any smaller change than this during test, assume limit has been reached (smaller changes assumed to be noise)
#define LONG_WAIT_MS 1200
#define SHORT_WAIT_MS 100
#define CALIB_AVERAGING_WINDOW_SIZE 300
#define TEST_MAX_ANGLE 1
#define TEST_MIN_ANGLE 0
#define PWM_AT_MAX_ANGLE 540 //540us, seems consistent across servos
#define PWM_AT_MIN_ANGLE 2400 //2400us, seems consistent across servos
#define PWM_US_RANGE (PWM_AT_MIN_ANGLE-PWM_AT_MAX_ANGLE)
#define PWM_AT_CENTRE_OF_TRAVEL 1470 //(PWM_AT_MAX_ANGLE + PWM_US_RANGE/2)

//Calibration variables
int pwmAtCentrePoint = PWM_AT_CENTRE_OF_TRAVEL;
int encoderValAtMaxAngle; //occurs at LOW pwm values
int encoderValAtMinAngle; //occurs at HIGH pwm values
int encoderRange;

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
  Serial.println("Type microsecond values to test. 600 and 2400 are reasonable endpoint estimations");
  Serial.println("Type 'e' to see the encoder reading at the current position");
  Serial.println("Type 'c' to move the horn to the set centre position");  
  Serial.println("Calibration step 1: Type 'k' to find upper/lower limits and centre the servo");
  Serial.println("Calibration step 2: Place the horn on the servo, in straightest available spline position");
  Serial.println("Calibration step 3: Use '[' and ']' to fine-adjust the position until the horn is straight");
  Serial.println("Calibration step 4: Type 'p' to print out the calibration parameters to be copied into the firmware.");
}

void loop() {

//Respond to individual character from serial or construct a string
while(Serial.available()) {
  //Read next character in
  recvdChar = Serial.read();

  switch (recvdChar) {
    //'e' to print current encoder reading
    case 'e' :
      //Report the encoder reading at this position
      encoderReading = getFilteredEncoderPos();
      Serial.print("Encoder reading at this position is: ");
      Serial.println(encoderReading);
      break;

    //'k' to find end limits and centre the servo
    case 'k' :
      //Find lower angle limit
      //This also updates the global min encoder value variable
      Serial.println("Finding lower angle limit...");
      joint_servo.writeMicroseconds(PWM_AT_MIN_ANGLE);
      delay(LONG_WAIT_MS);
      encoderValAtMinAngle = getFilteredEncoderPos();
      Serial.print("Encoder value at lower limit is: ");
      Serial.println(encoderValAtMinAngle);

      //Find upper angle limit
      //This also updates the global min encoder value variable
      Serial.println("Finding upper angle limit...");
      joint_servo.writeMicroseconds(PWM_AT_MAX_ANGLE);
      delay(LONG_WAIT_MS);
      encoderValAtMaxAngle = getFilteredEncoderPos();
      Serial.print("Encoder value at upper limit is: ");
      Serial.println(encoderValAtMaxAngle);

      //Calculate ranges for global encoder and PWM values
      encoderRange = encoderValAtMaxAngle - encoderValAtMinAngle;
      Serial.print("Total encoder range is: ");
      Serial.println(encoderRange);
      Serial.println("Moving to centre position...");
      Serial.println("Place horn and straighten up with [ and ], then press p to output calibration parameters");

      //Move to centre position
      joint_servo.writeMicroseconds(PWM_AT_CENTRE_OF_TRAVEL);
      break;

    //']' to increment angle (decrement PWM width)
    case ']' :
      //Move centrepoint and servo position in +ve direction
      pwmAtCentrePoint -= MANUAL_INCREMENT_US;
      joint_servo.writeMicroseconds(pwmAtCentrePoint);
      break;

    //'[' to decrement angle (increment PWM width)
    case '[' :
      //Move centrepoint and servo position in -ve direction
      pwmAtCentrePoint += MANUAL_INCREMENT_US;
      joint_servo.writeMicroseconds(pwmAtCentrePoint);
      break;

    //'p' to print out the calibrated parameters
    case 'p' :
      Serial.println("");
      Serial.print("#define CALIB_CENTRE_POSITION_US ");
      Serial.println(pwmAtCentrePoint);
      Serial.print("#define CALIB_ENCODER_VALUE_AT_MIN_ANGLE ");
      Serial.println(encoderValAtMinAngle);
      Serial.print("#define CALIB_ENCODER_VALUE_AT_MAX_ANGLE ");
      Serial.println(encoderValAtMaxAngle);
    break;

    //'c' to return to centre point, e.g. after using manual microsecond commands
    case 'c' :
      //Move to centre position
      joint_servo.writeMicroseconds(PWM_AT_CENTRE_OF_TRAVEL);
      break;

    //Number strings for manual microseconds sertting
    case '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' : case '9' :
      //If  is a number, carry on producing a string
      inputString += recvdChar; //add next char to string
      delay(2); //slow to allow next character to enter buffer
      break;

    default :
      Serial.print("Invalid character received: ");
      Serial.println(recvdChar);
  }
}

//Set PWM to specific number entered as text string
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
  getFilteredEncoderPos
  @brief Acquires multiple samples of encoder position and averages them to reduce noise
  This is slow and intended for calibration. A moving average would be used for a 'live' version
*/
int getFilteredEncoderPos() {
  float averagingTotal = 0;

  //Generate a summation of all the samples in the window
  for (int i=0; i < CALIB_AVERAGING_WINDOW_SIZE; ++i) {
        averagingTotal += analogRead(SERVO_POSITION_PIN);
  }
  //Average the summation over the window size to produce filtered result
  return averagingTotal/CALIB_AVERAGING_WINDOW_SIZE;
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

/*
  autoFindLimit
  @brief Automatically finds the endpoints of the servo using the encoder.
  @param testMaxNotMinAngle Define which endpoint to find (true = max angle, false = min angle)
*/
int autoFindLimit(bool testMaxNotMinAngle) {
  int currentPwmVal, newPwmVal;
  int currentEncoderVal, newEncoderVal, encoderValDifference;
  bool exitFlag = false;

  //Move to starting point
  currentPwmVal = testMaxNotMinAngle ? PWM_START_LO_US : PWM_START_HI_US;
  joint_servo.writeMicroseconds(currentPwmVal);
  delay(LONG_WAIT_MS);
  currentEncoderVal = analogRead(SERVO_POSITION_PIN);

  //Increment until encoder stops changing
  while(!exitFlag) {
    //Move servo another increment
    newPwmVal = testMaxNotMinAngle ? currentPwmVal - TEST_INCREMENT_US : currentPwmVal + TEST_INCREMENT_US;
    joint_servo.writeMicroseconds(newPwmVal);

    //Wait for movement and test position, filtered
    delay(SHORT_WAIT_MS);
    newEncoderVal = getFilteredEncoderPos();

    if(DEBUG) {
      Serial.print("Current (last) encoder value: ");
      Serial.print(currentEncoderVal);
      Serial.print(", New encoder value: ");
      Serial.println(newEncoderVal);
    }

    //If position hasn't changed, the last position was the limit
    encoderValDifference = newEncoderVal - currentEncoderVal;
    if (abs(encoderValDifference) < MIN_MEASURED_ENCODER_DIFFERENCE) {
      exitFlag = true; //Break out of this while loop
    } else {
      //Otherwise update last pwm and encoder values and go back round the loop
      currentEncoderVal = newEncoderVal;
      currentPwmVal = newPwmVal;
    }
  }

  //Update min or max encoder values. Note that these are counterintuitive.
  //Max encoder value occurs at minimum PWM pulsewidth and vice versa.
  if (testMaxNotMinAngle) {
    encoderValAtMaxAngle = currentEncoderVal;
  } else {
    encoderValAtMinAngle = currentEncoderVal;
  }

  //Once limit has been found, return it
  return currentPwmVal;
}
