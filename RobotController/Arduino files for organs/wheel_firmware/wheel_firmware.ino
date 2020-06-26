// Firmware for the wheel organ, with encoder. Authour: Matt
// This code is written for an Arduino Nano
#define SERIAL_DEBUG_PRINTING
//#define OPEN_LOOP_VALUES_FINDER
#define STEP_INPUT_TEST

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
float integral_value;

// encoder global variables
volatile int encoder_ticks_this_timestep;
volatile bool encoder_A_is_high=false;
volatile bool encoder_B_is_high=false;
volatile unsigned long time_of_last_tick=0;
volatile unsigned long time_of_last_but_one_tick=0;
volatile int last_tick_direction = 0;
float measured_velocity_ticks_per_timestep;

// data logging for step input test
#define N_POINTS_TO_LOG 100
int8_t data_log[N_POINTS_TO_LOG-1];
bool logging_running=false;
unsigned int i=0;

// interrupt service routines - these are called when the encoder ticks:
void A_rises(){
    encoder_A_is_high=true;
    if (encoder_B_is_high){increment_positive();}
    else {increment_negative();}
    attachInterrupt(PIN_ENCODER_A, A_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void A_falls(){
    encoder_A_is_high=false;
    if (encoder_B_is_high){increment_negative();}
    else {increment_positive();}
    attachInterrupt(PIN_ENCODER_A, A_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_rises(){
    encoder_B_is_high=true;
    if (encoder_A_is_high){increment_negative();}
    else {increment_positive();}
    attachInterrupt(PIN_ENCODER_B, B_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_falls(){
    encoder_B_is_high=false;
    if (encoder_A_is_high){encoder_ticks_this_timestep ++;}
    else {encoder_ticks_this_timestep --;}
    attachInterrupt(PIN_ENCODER_B, B_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
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

// from the raw 8-bit input value, extract the motor state and demand velocity values
void interpret_control_register_value(){
  // extract motor state bits:
  motor_state = raw_input_for_control_register & 0x03; // last 2 bits
  // extract the motor speed bits:
  uint8_t speed_value_raw = raw_input_for_control_register >> 2; // first 6 bits
  
  if (motor_state == FORWARD) { demand_velocity = speed_value_raw; }
  else if (motor_state == REVERSE) { demand_velocity = -speed_value_raw; }
  else { demand_velocity = 0; } // either BRAKE or STANDBY

#ifdef SERIAL_DEBUG_PRINTING// debugging output:
//    Serial.print("Received value as binary: ");
//    for (int bit=7;bit>=0;bit--){
//        Serial.print( raw_input_for_control_register>>bit & 0x01 );
//    }
//    Serial.println();
//    Serial.print("velocity as binary: ");
//    for (int bit=5;bit>=0;bit--){
//        Serial.print( speed_value_raw>>bit & 0x01 );
//    }
    Serial.println();
    Serial.print("New motor state :");
    Serial.println(motor_state);
    Serial.print("New demand velocity:");
    Serial.println(demand_velocity);
    Serial.print("OL controller value:");
    Serial.println(controller( demand_velocity, demand_velocity ));
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
//    float temp_value = (1.0/30) * 1000000.0/ (time_of_last_tick-time_of_last_but_one_tick) * last_tick_direction;
//    last_tick_direction=0;
//    return temp_value;


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
# define OPEN_LOOP_CONSTANT 2.5
# define OPEN_LOOP_MULTIPLE 7.248
#define K_P 5.0
#define K_I 0.0
int sign(int number){
    if (number==0){return 0;}
    else if (number<0){return -1;}
    else{ return 1;}
}
int controller( int demand_velocity, int measured_velocity ){
    // open-loop controller
    float controller_value;
    if (demand_velocity==0){
        controller_value=0;
    }else{
        controller_value = sign(demand_velocity)*OPEN_LOOP_CONSTANT + demand_velocity*OPEN_LOOP_MULTIPLE;
    }

    // compute new integral_value
    integral_value += K_I* (demand_velocity-measured_velocity);

    // add on proportional and integral components:
    controller_value += K_P*(demand_velocity-measured_velocity) + integral_value;

    if (abs(controller_value)>255){return sign(controller_value)*255;}
    else {return int(controller_value);}
}

void set_motor_power_value(int value){
    if (value<0){
        // negative
        digitalWrite(PIN_DRIVER_AIN1,LOW);
        digitalWrite(PIN_DRIVER_AIN2,HIGH);
        analogWrite(PIN_DRIVER_APWM,-value);
    }
    else if (value>0){
        // positive
        digitalWrite(PIN_DRIVER_AIN1,HIGH);
        digitalWrite(PIN_DRIVER_AIN2,LOW);
        analogWrite(PIN_DRIVER_APWM,value);
    } else { //zero
        digitalWrite(PIN_DRIVER_APWM,0);
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

    //encoder start
    attachInterrupt(PIN_ENCODER_A, A_rises, RISING);
    attachInterrupt(PIN_ENCODER_B, B_rises, RISING);

    // start i2c comms
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);

    // prepare for loop start
    loop_target_end_time = micros();

    #ifdef OPEN_LOOP_VALUES_FINDER
        // should not normally be run. This applied a series of fixed values, and sees what the restuling steady state velocity is
        Serial.begin(115200);
        Serial.println("Open loop test...");
        Serial.println("power,velocity");
        enable_motor();
        for(int power=-240;power<255;power+=16) {
            brake_motor();
            delay(500);
            set_motor_power_value(power);
            delay(2500); // time to settle to steady state
            Serial.print(power);
            get_measured_velocity_from_encoder();
            loop_target_end_time=micros()+LOOP_TIME_US;
            for (int i = 0; i < 40; i++){

                // wait until it is time for next loop
                unsigned long time_now = micros();
                if (loop_target_end_time - time_now > 16380) // delayMicroseconds is limited to 16383
                { delay( (loop_target_end_time - time_now )/1000 );}
                else{delayMicroseconds(loop_target_end_time - time_now); }
                loop_target_end_time = loop_target_end_time+ LOOP_TIME_US;

                Serial.print(",");
//                Serial.print(micros());
                Serial.print(get_measured_velocity_from_encoder());
            }
            Serial.println();
        }
        brake_motor();
        while(true){delay(1000);} // hang
    #endif
    #ifdef SERIAL_DEBUG_PRINTING//
        Serial.begin(115200); // for debugging only
        Serial.println("I am a wheel organ...");
    #endif
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
    measured_velocity_ticks_per_timestep = get_measured_velocity_from_encoder(); // this should happen as soon as possible after the delay
//    #ifdef SERIAL_DEBUG_PRINTING
//        Serial.println(measured_velocity_ticks_per_timestep);
//    #endif

    #ifdef STEP_INPUT_TEST   // Data logging:
        if (logging_running){
            data_log[i] = measured_velocity_ticks_per_timestep; // save value
            i++;
            if (i>=N_POINTS_TO_LOG){
                logging_running = false;
                Serial.println("================");
                Serial.print("Demand value was: ");
                Serial.println(demand_velocity);
                Serial.println("================");
                for (i = 0; i < N_POINTS_TO_LOG; i++) {
                    Serial.println(data_log[i]);
                }
                i=0;
            }
        }
    #endif

  // check for new demand value:
  if(new_control_register_value_received_flag){
    new_control_register_value_received_flag=false;
    #ifdef STEP_INPUT_TEST
      logging_running=true;
    #endif
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
//        #ifdef SERIAL_DEBUG_PRINTING//
//          Serial.println(raw_motor_power_value);
//        #endif
    }
}
