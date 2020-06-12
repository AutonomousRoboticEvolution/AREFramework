// Firmware for the wheel organ, with encoder. Authour: Matt
// This code is written for an Arduino Nano
#define SERIAL_DEBUG_PRINTING

#include <Wire.h>

// This code should interface with Mike's driver code, and so mimics the register(s) used by the i2c motor driver (DRV8830):
#define DRV_CONTROL_REG_ADDR 0x00
#define DRV_FAULT_REG_ADDR 0x01 // not (yet?) implemented

#define SLAVE_ADDRESS 0x20

//IN1 and IN2 states
#define STANDBY 0x0
#define REVERSE 0x1
#define FORWARD 0x2
#define BRAKE 0x3

//CONTROL register fields
// [7:2] motorSpeed bits
// [1:0] motorState bits

// pin definitions (Arduino Nano pins):
#define PIN_ENCODER_A 0 // THIS IS PIN 2 - see table at bottom of this page: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
#define PIN_ENCODER_B 1 // THIS IS PIN 3
#define PIN_DRIVER_AIN1 8
#define PIN_DRIVER_AIN2 7
#define PIN_DRIVER_APWM 6
#define PIN_DRIVER_ENABLE 9

// define controller settings:
#define LOOP_TIME_US 10000 // will set the controller update frequency; 10,000us -> 100Hz

// flags and global variables:
bool is_enabled = false;
bool new_control_register_value_received_flag = false;
uint8_t raw_input_for_control_register;
int motor_power;
unsigned long loop_target_end_time;
int motor_state; //will be either STANDBY, REVERSE, FORWARD or BRAKE defined above
int demand_velocity;
uint8_t input_buffer[2];

// encoder global variables
volatile int encoder_ticks_this_timestep;
volatile bool encoder_A_is_high=false;
volatile bool encoder_B_is_high=false;
int measured_velocity_ticks_per_timestep;

// interrupt service routines - these are called when the encoder ticks:
void A_rises(){
    encoder_A_is_high=true;
    if (encoder_B_is_high){encoder_ticks_this_timestep ++;}
    else {encoder_ticks_this_timestep --;}
    attachInterrupt(PIN_ENCODER_A, A_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void A_falls(){
    encoder_A_is_high=false;
    if (encoder_B_is_high){encoder_ticks_this_timestep --;}
    else {encoder_ticks_this_timestep ++;}
    attachInterrupt(PIN_ENCODER_A, A_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_rises(){
    encoder_B_is_high=true;
    if (encoder_A_is_high){encoder_ticks_this_timestep --;}
    else {encoder_ticks_this_timestep ++;}
    attachInterrupt(PIN_ENCODER_B, B_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_falls(){
    encoder_B_is_high=false;
    if (encoder_A_is_high){encoder_ticks_this_timestep ++;}
    else {encoder_ticks_this_timestep --;}
    attachInterrupt(PIN_ENCODER_B, B_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}

// from the raw 8-bit input value, extract the motor state and demand velocity values
void interpret_control_register_value(){
  // extract motor state bits:
  motor_state = raw_input_for_control_register & 0x03; // last 2 bits
  // extract the motor speed bits:
  uint8_t speed_value_raw = raw_input_for_control_register & 0xFC >> 2; // first 7 bits
  
  if (motor_state == FORWARD) { demand_velocity = speed_value_raw; }
  else if (motor_state == REVERSE) { demand_velocity = -speed_value_raw; }
  else { demand_velocity = 0; } // either BRAKE or STANDBY

#ifdef SERIAL_DEBUG_PRINTING// debugging output:
  Serial.println("New motor state and demand velocity are:");
  Serial.println(motor_state);
  Serial.println(demand_velocity);
#endif
}

// triggered on every i2c conversation:
void receiveData(int received_data_byte_count){

  // the first byte received is the register value, and determines what we should do
  input_buffer[0] = Wire.read(); // register
  if (received_data_byte_count>1){
    input_buffer[1] = Wire.read(); // value
  }else{
    input_buffer[1] = 0;
  }
//  debugging:
#ifdef SERIAL_DEBUG_PRINTING//
  Serial.print("Received: 0x");
  Serial.print(input_buffer[0],HEX);
  Serial.print(", 0x");
  Serial.println(input_buffer[1],HEX);
#endif

  switch (input_buffer[0]) {
    case DRV_CONTROL_REG_ADDR: // set control register
      raw_input_for_control_register = input_buffer[1];
      new_control_register_value_received_flag = true;
      break;
  
    default:
//      Serial.print("Attempt to access unkown register: 0x");
//      Serial.println(input_buffer[0], HEX);
      break;
  }
}     

int get_measured_velocity_from_encoder(){
    int temp_value = encoder_ticks_this_timestep;
    encoder_ticks_this_timestep=encoder_ticks_this_timestep-temp_value;
    return temp_value;
}

void disable_motor(){
  if (is_enabled){
    is_enabled=false;
    digitalWrite(PIN_DRIVER_ENABLE, LOW);
  }
}

void enable_motor(){
    if (!is_enabled){
        is_enabled=true;
        digitalWrite(PIN_DRIVER_ENABLE, HIGH);
    }
}

void brake_motor(){
    enable_motor();
    digitalWrite(PIN_DRIVER_AIN1,HIGH);
    digitalWrite(PIN_DRIVER_AIN2,HIGH);
    digitalWrite(PIN_DRIVER_APWM,0);
}

// will return the raw motor power value, in the range -255 to +255
int controller( int demand_velocity, int measured_velocity ){
    // to do properly, for now just output proportional to demand value
    return demand_velocity * 4;
}

void set_motor_power_value(int value){
    if (value<0){
        // negative
        digitalWrite(PIN_DRIVER_AIN1,LOW);
        digitalWrite(PIN_DRIVER_AIN2,HIGH);
        analogWrite(PIN_DRIVER_APWM,-value);
    }
    else {
        // positive
        digitalWrite(PIN_DRIVER_AIN1,HIGH);
        digitalWrite(PIN_DRIVER_AIN2,LOW);
        analogWrite(PIN_DRIVER_APWM,value);
    }
}

void setup(){
    // set the pin modes
    pinMode(PIN_DRIVER_AIN1, OUTPUT);
    pinMode(PIN_DRIVER_AIN2, OUTPUT);
    pinMode(PIN_DRIVER_APWM, OUTPUT);
    pinMode(PIN_DRIVER_ENABLE, OUTPUT);
    pinMode(encoder_A_is_high, INPUT);
    pinMode(encoder_B_is_high, INPUT);

    digitalWrite(PIN_DRIVER_ENABLE, LOW); // start not enabled
    #ifdef SERIAL_DEBUG_PRINTING//
        Serial.begin(115200); // for debugging only
        Serial.println("I am a wheel organ...");
    #endif

    //encoder start
    attachInterrupt(PIN_ENCODER_A, A_rises, RISING);
//    attachInterrupt(PIN_ENCODER_A, A_falls, FALLING);
    attachInterrupt(PIN_ENCODER_B, B_rises, RISING);
//    attachInterrupt(PIN_ENCODER_B, B_falls, FALLING);

    // start i2c comms
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);

    // prepare for loop start
    loop_target_end_time = micros();
}

void loop(){
    // wait until it is time for next loop
    unsigned long time_now = micros();
    loop_target_end_time += LOOP_TIME_US;
    if (loop_target_end_time > time_now) {
        if (loop_target_end_time - time_now > 16380) // delayMicroseconds is limited to 16383
        { delay( (loop_target_end_time - time_now )/1000 );}
        else{delayMicroseconds(loop_target_end_time - time_now); }
    }else{ // couldn't keep up
        #ifdef SERIAL_DEBUG_PRINTING
            Serial.println("can't keep up");
        #endif
        loop_target_end_time = time_now+ LOOP_TIME_US;
    }
    measured_velocity_ticks_per_timestep = get_measured_velocity_from_encoder(); // this should happen as soon as possible after the delay, because it assumes the correct time interval between calls
    #ifdef SERIAL_DEBUG_PRINTING
        Serial.println(measured_velocity_ticks_per_timestep);
    #endif

  // check for new demand value:
  if(new_control_register_value_received_flag){
    new_control_register_value_received_flag=false;
    interpret_control_register_value();
  }

  // depending on motor_state, decide what to do:
  if (motor_state == STANDBY){
      disable_motor();
  }
  else if (motor_state == BRAKE){
      brake_motor();
  }
  else { // must be FORWARD or REVERSE
        enable_motor();
        int raw_motor_power_value = controller(demand_velocity, measured_velocity_ticks_per_timestep);
        set_motor_power_value(raw_motor_power_value);
    }
}
