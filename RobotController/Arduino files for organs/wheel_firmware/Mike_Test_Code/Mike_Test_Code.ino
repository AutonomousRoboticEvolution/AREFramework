#include <Wire.h>

#define INDICATOR 9
#define MOTOR_PWM_PIN 5
#define MOTOR_DIR_PIN 6
#define ENC_A_PIN 2
#define ENC_B_PIN 3
#define I2CENABLE 15

#define SWEEP_STEP_PERIOD 10
#define SLAVE_ADDRESS 0x07

// encoder global variables
volatile int encoder_ticks_this_timestep;
volatile bool encoder_A_is_high=false;
volatile bool encoder_B_is_high=false;
volatile unsigned long time_of_last_tick=0;
volatile unsigned long time_of_last_but_one_tick=0;
volatile int last_tick_direction = 0;
float measured_velocity_ticks_per_timestep;

bool endFlag = 0; //for indicator LED test

// interrupt service routines - these are called when the encoder ticks:
void A_rises(){
//    Serial.println("A^");
    encoder_A_is_high=true;
    if (encoder_B_is_high){increment_positive();}
    else {increment_negative();}
    attachInterrupt(ENC_A_PIN, A_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void A_falls(){
    encoder_A_is_high=false;
    if (encoder_B_is_high){increment_negative();}
    else {increment_positive();}
    attachInterrupt(ENC_A_PIN, A_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_rises(){
//    Serial.println("B^");
    encoder_B_is_high=true;
    if (encoder_A_is_high){increment_negative();}
    else {increment_positive();}
    attachInterrupt(ENC_B_PIN, B_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_falls(){
    encoder_B_is_high=false;
    if (encoder_A_is_high){encoder_ticks_this_timestep ++;}
    else {encoder_ticks_this_timestep --;}
    attachInterrupt(ENC_B_PIN, B_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
// helper functions for incrementing/decrementing the encoder tracking:
void increment_positive(){
    last_tick_direction=1;
    encoder_ticks_this_timestep ++;
    time_of_last_but_one_tick = time_of_last_tick;
    time_of_last_tick=micros();
}
void increment_negative(){
    last_tick_direction=-1;
    encoder_ticks_this_timestep --;
    time_of_last_but_one_tick = time_of_last_tick;
    time_of_last_tick=micros();
}

int get_measured_velocity_from_encoder(){
    int temp_value = encoder_ticks_this_timestep;
    encoder_ticks_this_timestep=encoder_ticks_this_timestep-temp_value;
    return temp_value;
//    float temp_value = (1.0/30) * 1000000.0/ (time_of_last_tick-time_of_last_but_one_tick) * last_tick_direction;
//    last_tick_direction=0;
//    return temp_value;


}

//Setup function
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(INDICATOR, OUTPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(ENC_A_PIN, INPUT);
  pinMode(ENC_B_PIN, INPUT);

  //encoder start
  attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), A_rises, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), B_rises, RISING);

  //Serial start
  Serial.begin(9600);
  Serial.println("Wheel Organ Reporting");

  //I2C slave setup
  Wire.begin(SLAVE_ADDRESS);
//  Wire.onReceive(receiveData);
//  Wire.onRequest(sendData);
  Serial.println("I2C Ready!");

  //TESTCODE Open the I2C channel to the outside world (active-low)
  pinMode(I2CENABLE, OUTPUT);
  digitalWrite(I2CENABLE, LOW);
  Serial.println("I2C to outside world enabled");
  


}

//Main loop
void loop() {

  //Fade in
  digitalWrite(MOTOR_DIR_PIN, 1);
  while(!endFlag) {
    for(int pwmVal = 0; pwmVal <256; pwmVal++) {
      analogWrite(INDICATOR, pwmVal);
      analogWrite(MOTOR_PWM_PIN, pwmVal);
      delay(SWEEP_STEP_PERIOD);
      Serial.println(encoder_ticks_this_timestep);
    }
    endFlag = true;
  }
  endFlag = false;
  //Fade out
  while(!endFlag) {
    for(int pwmVal = 255; pwmVal >=0; pwmVal--) {
      analogWrite(INDICATOR, pwmVal);
      analogWrite(MOTOR_PWM_PIN, pwmVal);
      delay(SWEEP_STEP_PERIOD);
      Serial.println(encoder_ticks_this_timestep);
    }
    endFlag = true;
  }
  endFlag = false;
  //Fade in reverse
  digitalWrite(MOTOR_DIR_PIN, 0);
  while(!endFlag) {
    for(int pwmVal = 0; pwmVal <256; pwmVal++) {
      analogWrite(INDICATOR, pwmVal);
      analogWrite(MOTOR_PWM_PIN, pwmVal);
      delay(SWEEP_STEP_PERIOD);
      Serial.println(encoder_ticks_this_timestep);
    }
    endFlag = true;
  }
  endFlag = false;
  //Fade out
  while(!endFlag) {
    for(int pwmVal = 255; pwmVal >=0; pwmVal--) {
      analogWrite(INDICATOR, pwmVal);
      analogWrite(MOTOR_PWM_PIN, pwmVal);
      delay(SWEEP_STEP_PERIOD);
      Serial.println(encoder_ticks_this_timestep);
    }
    endFlag = true;
  }
  endFlag = false;

}
