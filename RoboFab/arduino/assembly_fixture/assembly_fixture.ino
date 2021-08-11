// Libraries
#include "Keyboard.h"

#define DEGREES_PER_STEP 1.8 //1.8 for our 12v stepper, 0.9 for the earlier 2.8v stepper
#define WORM_GEAR_RATIO 50  //Rotations of worm gear for one rotation of wheel gear
#define ACCEL_INCREMENT_US 4 //Step duration changed by this much per step when accelerating [2] <-brackets indicate good settings for 2.8V stepper [12V: 4]
#define STEP_MIN_DURATION_US 1500 //Shortest pulse width (defines maximum rotation speed) [700] [12V: 1200]
#define STEP_MAX_DURATION_US 2000 //Longest pulse width (defines minimum, i.e. start and end speed)[1200] [12V: 2000]
#define STEP_HOMING_DURATION_US 2000 //Extra slow speed for the last bit of homing [1600] [12V 3000]
//Note that these durations relate to the total step time; on-time/off-time will be half these values
#define ACCEL_RAMP_STEPS ((STEP_MAX_DURATION_US - STEP_MIN_DURATION_US)/ACCEL_INCREMENT_US) //Steps required to transition between min and max speed

#define HOMING_RETREAT_DEGREES 10 //Homing routine retreats this much from endstop before doing final homing pass
#define HOME_POSITION_DEG (360 - 235.5) //Starting position relative to angle at which endstop is triggered

#define DEBUG 0 //To enable or disable debug messages

// Constants - pins 
const int LEDPin = 2;
const int electromagnetsPin = 3;
const int motorStepPin = 4;
const int motorDirectionPin = 5;
const int enablePin = 6;
const int endstopPin = 7;

// Global variables
float memRot; // Keeps track of the rotation of the robot
String inString = ""; // Stores message received from serial

// Set-up
void setup() {
  // Define inputs and outputs
  pinMode(LEDPin, OUTPUT);
  pinMode(electromagnetsPin, OUTPUT);
  pinMode(motorStepPin, OUTPUT);
  pinMode(motorDirectionPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(endstopPin, INPUT);
  disableOutputs();
  // Open the serial port:
  Serial.begin(9600);
  Serial.println("I am the Assembly Fixture");
  memRot = 0;
}

void loop() {
  // Check for incoming serial data:
  if(Serial.available() > 0){
    // Read incoming serial data
    inString = Serial.readString();
    char firstChar = inString.charAt(0);
    // If the first character is a digit (or a negative sign), do rotation
    if(isDigit(firstChar) || firstChar == '-'){
      rotateToDegrees(inString.toFloat());
      Serial.write("OK\n");
    }
    else {
      switch(firstChar){
        // Hold (h) robot
        case 'h':
          turnOnElectromagnets();
          turnOnLED();
          Serial.write("OK\n");
          break;
        // Release (r) robot
        case 'r':
          turnOffElectromagnets();
          turnOffLED();
          Serial.write("OK\n");
          break;
        // Reset current position memory (m)
        case 'm':
          memRot = 0;
          Serial.write("OK\n");
          break;
        // Enable outputs (e - enable)
        case 'e':
          enableOutputs();
          Serial.write("OK\n");
          break;
        // Disable outputs (o - off)
        case 'o':
          disableOutputs();
          Serial.write("OK\n");
          break;
        case 'f':
          findOrigin();
          Serial.write("OK\n");
          break;
        default:
          Serial.write("error - unrecognised input\n");
          break;
      }
    }
  }
  inString = "";
}
void turnOnElectromagnets(){
  digitalWrite(electromagnetsPin, HIGH);
}
void turnOffElectromagnets(){
  digitalWrite(electromagnetsPin, LOW);
}
void turnOnLED(){
  digitalWrite(LEDPin, HIGH);
}
void turnOffLED(){
  digitalWrite(LEDPin, LOW);
}
// Set direction of rotation CW
void setMotorCW(){
  digitalWrite(motorDirectionPin, HIGH);
}
// Set direction of rotation CCW
void setMotorCCW(){
  digitalWrite(motorDirectionPin, LOW);
}
// Enable outputs to motor
void enableOutputs(){
  digitalWrite(enablePin, LOW);
}
// Disable outputs to motor
void disableOutputs(){
  digitalWrite(enablePin, HIGH);
}
// Rotate to a certain angle
void rotateToDegrees(float deg){
  //Report target location if debugging
  if (DEBUG) Serial.print("Rotating to ");
  if (DEBUG) Serial.print(deg);
  if (DEBUG) Serial.println();

  //Constrain target angle to range -360 to 360 degrees
  float newDeg = floatRemainder(deg, 360);
  //Constrain target angle to range 0 to 360 degrees (i.e. add 360 if negative)
  newDeg = (newDeg < 0) ? (newDeg + 360) : newDeg;

  //Identify which of the current or target angles is larger than the other
  //(simplifies calculation of the two possible routes between them)
  float largerAngle = (newDeg > memRot) ? newDeg : memRot;
  float smallerAngle = (newDeg < memRot) ? newDeg : memRot;

  //Calculate the two possible rotations to move to newDeg
  //All angles are now constrained to range 0-360 degrees
  //The result of both calculations will be a positive value indicating the magnitude of offset
  float rotationA = (360.0f - largerAngle) + smallerAngle;
  float rotationB = largerAngle - smallerAngle;

  //Identify the direction of each rotation, which depends on which of newDeg and memRot was the largerAngle
  //If newDeg was the largerAngle, rotationB is clockwise, otherwise rotationA is clockwise
  float cwRotation = (newDeg > memRot) ? rotationB : rotationA;
  float ccwRotation = (newDeg > memRot) ? rotationA : rotationB;

  //Choose the shortest rotation and perform it
  int steps = 0;
  if (cwRotation < ccwRotation) {
    setMotorCW();
    steps = degreesToSteps(cwRotation);
    memRot += stepsToDegrees(steps);  //Update memRot with actual degrees moved, clockwise = positive offset
  } else {
    setMotorCCW();
    steps = degreesToSteps(ccwRotation);
    memRot -= stepsToDegrees(steps);  //Update memRot with actual degrees moved, anticlockwise = negative offset
  }
  advanceStepper(steps, false); //Perform the rotation, not in homing mode

  //New memRot might be outside the range 0-360; check for this and  correct it
  memRot = (memRot < 0) ? (memRot + 360) : memRot;
  memRot = (memRot > 360) ? (memRot - 360) : memRot;

  //Report actual location if debugging
  if (DEBUG) Serial.print("Rotated to ");
  if (DEBUG) Serial.print(memRot);
  if (DEBUG) Serial.println();
}

// Advance the stepper by specified number of steps
// This provides for acceleration/deceleration
// Setting the 2nd argument to TRUE will stop pulsing when endstop is reached
void advanceStepper(int numSteps, bool isHoming) {
  int rampUpSteps = 0;
  int topSpeedSteps = 0;
  int rampDownSteps = 0;
  
  //Calculate number of ramping steps for accel/decel first
  //If too few steps, we won't reach top speed
  if (numSteps < ACCEL_RAMP_STEPS*2) {
    rampUpSteps = numSteps/2; //Integer division, any remainder would be discarded
    rampDownSteps = numSteps/2; //Integer division, any remainder would be discarded
    topSpeedSteps = numSteps - rampUpSteps - rampDownSteps; //Will either be zero or 1 if numSteps was odd
  } else { //Otherwise do full ramp up and down, all remaining steps in between at top speed
    rampUpSteps = ACCEL_RAMP_STEPS;
    rampDownSteps = ACCEL_RAMP_STEPS;
    topSpeedSteps = numSteps - rampUpSteps - rampDownSteps;
  }

  //Starting from minimum speed, perform ramp up, top speed, and ramp down
  //If this is part of the homing routine, stop stepping as soon as origin is found
  int pulseDuration = STEP_MAX_DURATION_US;
  ///Accelerate from min to max speed
  for (int i=0; i<rampUpSteps; ++i) {
    //Check for endstop if homing
    if(isHoming) {
      if(isOriginFound()) {
        break;
      }
    }
    //Perform step and adjust pulse duration for next step
    step(pulseDuration);
    pulseDuration -= ACCEL_INCREMENT_US;
  }
  //Perform full speed steps, if any
  for (int i=0; i<topSpeedSteps; ++i) {
    //Check for endstop if homing
    if(isHoming) {
      if(isOriginFound()) {
        break;
      }
    }
    //Perform step
    step(pulseDuration);
  }
  //Decelerate from max to min speed
  for (int i=0; i<rampDownSteps; ++i) {
    //Check for endstop if homing
    if(isHoming) {
      if(isOriginFound()) {
        break;
      }
    }
    //Adjust pulse duration for next step and then perform it
    pulseDuration += ACCEL_INCREMENT_US;
    step(pulseDuration);
  }
}

//Increment motor position by a single step of specified duration
//Note the duration is the total step time, so the on-time and off-time are half each
void step(int stepDuration) {
  digitalWrite(motorStepPin, LOW);
  delayMicroseconds(stepDuration/2);
  digitalWrite(motorStepPin, HIGH);
  delayMicroseconds(stepDuration/2);
}

// Rotate assembly fixture until find origin
void findOrigin(){

  // enable the stepper motor (if it isn't already)
  enableOutputs();
  
  //Rotate quickly clockwise until endstop hit
  if(DEBUG) Serial.println("Trying to home...");  
  setMotorCW();
  int numSteps = degreesToSteps(360); //Ensures will travel far enough to find endstop
  advanceStepper(numSteps, true); //Advance the stepper in homing mode; i.e. will return when endstop is hit
  
  //Retreat slightly for the more precise pass
  if(DEBUG) Serial.println("Hit endstop, retreating...");
  setMotorCCW();
  numSteps = degreesToSteps(HOMING_RETREAT_DEGREES);
  advanceStepper(numSteps, false);  //Move back by the retreat amount

  //Move slowly clockwise again until endstop found
  if(DEBUG) Serial.println("Finishing homing...");
  setMotorCW();
  while(!isOriginFound()) {
    step(STEP_HOMING_DURATION_US);
  }
  
  //Position is now known, we are at the endstop, so set the home position
  if(DEBUG) Serial.println("Found home");
  memRot = HOME_POSITION_DEG; //Ensure starting position is an offset from this location
}

// Detect origin
bool isOriginFound(){
  bool originFound;
  originFound = digitalRead(endstopPin);
  return originFound;
}

//Convert degrees into steps
int degreesToSteps(float degrees) {
  return (degrees / DEGREES_PER_STEP) * WORM_GEAR_RATIO;
}

//Convert steps into degrees
float stepsToDegrees(int steps) {
  return ((float) DEGREES_PER_STEP * steps)/WORM_GEAR_RATIO;
}

//Find the remainder of a float number when divided
//This is intended to be a float equivalent to the % operator in C
//The sign of the result will always be the same as the sign of the operand
float floatRemainder(float operand, float divisor) {
  //Cumulatively subtract the divisor from the operand until it is no longer divisible by that value
  while (abs(operand) >= abs(divisor)) {
    //If operand is positive, subtract the divisor, otherwise add the divisor
    operand = (operand > 0) ? (operand - abs(divisor)) : (operand + abs(divisor));
  }
  return operand;
}
