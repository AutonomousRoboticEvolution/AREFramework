// Firmware for the wheel organ, with encoder. Authour: Matt
// This pin assignments are the same as for the arduino Uno
//#define SERIAL_DEBUG_PRINTING
//#define OPEN_LOOP_VALUES_FINDER
//#define STEP_INPUT_TEST

#define SLAVE_ADDRESS 0x61 // <=== THIS NEEDS TO BE SET FOR EACH UNIQUE ORGAN

#include <Wire.h>

// This code should interface with Mike's driver code, and so mimics the register(s) used by the i2c motor driver (DRV8830):
#define DRV_CONTROL_REG_ADDR 0x00
#define DRV_FAULT_REG_ADDR 0x01 // not (yet?) implemented

// states defined by the 0th and 1st bits of a command signal
#define STANDBY 0x0
#define REVERSE 0x1
#define FORWARD 0x2
#define BRAKE 0x3

//CONTROL register fields
// [7:2] motorSpeed bits
// [1:0] motorState bits

// pin definitions (Arduino Uno pins):
#define PIN_ENCODER_A 0 // THIS IS PIN 2 - see table at bottom of this page: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
#define PIN_ENCODER_B 1 // THIS IS PIN 3
#define PIN_MOTOR_PWM_PIN 5
#define PIN_MOTOR_DIR_PIN 6
#define PIN_I2CENABLE 15
#define PIN_INDICATOR_LED 9

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

#ifdef STEP_INPUT_TEST
// data logging for step input test
#define N_POINTS_TO_LOG 50
int8_t data_log[N_POINTS_TO_LOG-1];
int8_t data_log_inputs[N_POINTS_TO_LOG-1];
bool logging_running=false;
unsigned int i=0;
#endif


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
    Serial.println(FF_controller( demand_velocity ));
#endif
}


// triggered on every i2c conversation:
void receiveData(int received_data_byte_count){
  // the first byte received is the register value, and determines what we should do
    input_buffer[0] = Wire.read(); // register
  #ifdef SERIAL_DEBUG_PRINTING
    Serial.print("Received: 0x");
    Serial.print(input_buffer[0],HEX);
  #endif
  if (received_data_byte_count>1){
    input_buffer[1] = Wire.read(); // value
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print(", 0x");
      Serial.println(input_buffer[1],HEX);
    #endif
  }else{
    input_buffer[1] = 0;
  }

  switch (input_buffer[0]) {
    case DRV_CONTROL_REG_ADDR: // set control register
      raw_input_for_control_register = input_buffer[1];
      new_control_register_value_received_flag = true;
      break;
  
    default:
      #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("Attempt to access unknown register: 0x");
        Serial.println(input_buffer[0], HEX);
      #endif
      break;
  }
}
 

int get_measured_velocity_from_encoder(){
    int temp_value = encoder_ticks_this_timestep;
    encoder_ticks_this_timestep=encoder_ticks_this_timestep-temp_value;
    return temp_value;
}


// controller parameters
#define K_P 10.0
#define K_I 0.5
#define MAX_INTEGRAL_VALUE 150 // for anti-windup, the integral term will never have magnitude greater than this
int sign(int number){
    if (number==0){return 0;}
    else if (number<0){return -1;}
    else{ return 1;}
}

// will return the raw motor power value, in the range -255 to +255
int controller( int demand_velocity, int measured_velocity ){
    // open-loop controller
    float controller_value;    
    controller_value = FF_controller(demand_velocity);

    // compute new integral_value
    integral_value += K_I* (demand_velocity-measured_velocity);
    integral_value = constrain(integral_value,-MAX_INTEGRAL_VALUE,MAX_INTEGRAL_VALUE);

    // add on proportional and integral components:
    controller_value += K_P*(demand_velocity-measured_velocity) + integral_value;

    if (abs(controller_value)>255){return sign(controller_value)*255;}
    else {return int(controller_value);}
}

// will return the raw motor power value, in the range -255 to +255, using only the a lookup table to make a feedforward controller
float FF_controller(int demand_velocity){
int table[][2] = {{0, 0}, {40,8}, {80,23}, {120,29}, {240,35}};
int x0, x1, y0, y1;
for (int i = 0; i < 5; i++){ // loop through each of the "brackets" of the table
  if ( abs(demand_velocity) <= table[i + 1][1]){ // we are in the right bracket
    y0 = table[i][1];  //lower bound
      y1 = table[i + 1][1]; //upper bound
      x0 = table[i][0];
      x1 = table[i + 1][0];
      return sign(demand_velocity) * float(table[i][0] + ((table[i + 1][0] - table[i][0] ) * (float(abs(demand_velocity) - table[i][1]) / float(table[i + 1][1] - table[i][1])))); // linear interpolation
      //                                  (x0          + ((x1              - x0          ) * (     (y                    - y0         ) /      (y1              - y0         ))));
    }
  }
  // if we get here, demand must be greater than highest table value, so return max power
  return sign(demand_velocity) * 240;
}

void set_motor_power_value(int value){
    if (value<0){
        // negative
        digitalWrite(PIN_MOTOR_DIR_PIN,LOW);
        analogWrite(PIN_MOTOR_PWM_PIN,-value);
    }
    else if (value>0){
        // positive
        digitalWrite(PIN_MOTOR_DIR_PIN,HIGH);
        analogWrite(PIN_MOTOR_PWM_PIN,value);
    } else { //zero
        digitalWrite(PIN_MOTOR_PWM_PIN,0);
    }
}


void setup(){
    // set the pin modes
    pinMode(PIN_MOTOR_DIR_PIN, OUTPUT);
    pinMode(PIN_MOTOR_PWM_PIN, OUTPUT);
    pinMode(PIN_INDICATOR_LED, OUTPUT);
    pinMode(PIN_I2CENABLE, OUTPUT);

    //encoder start
    attachInterrupt(PIN_ENCODER_A, A_rises, RISING);
    attachInterrupt(PIN_ENCODER_B, B_rises, RISING);

    // start i2c comms
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);

    // prepare for loop start
    loop_target_end_time = micros();

    #ifdef OPEN_LOOP_VALUES_FINDER
        // should not normally be run. This applied a series of fixed values, and sees what the resulting steady state velocity is
        Serial.begin(115200);
        Serial.println("Open loop test...");
        Serial.println("power,velocity");
        for(int power=-240;power<=240;power+=5) {
            set_motor_power_value(0);
            delay(500);
            set_motor_power_value(power);
            delay(2500); // time to settle to steady state
            Serial.print(power);
            get_measured_velocity_from_encoder();
            loop_target_end_time=micros()+LOOP_TIME_US;
            for (int i = 0; i < 10; i++){

                // wait until it is time for next loop
                unsigned long time_now = micros();
                if (loop_target_end_time - time_now > 16380) // delayMicroseconds is limited to 16383
                { delay( (loop_target_end_time - time_now )/1000 );}
                else{delayMicroseconds(loop_target_end_time - time_now); }
                loop_target_end_time = loop_target_end_time+ LOOP_TIME_US;

                Serial.print(",");
                Serial.print(get_measured_velocity_from_encoder());
            }
            Serial.println();
        }
        set_motor_power_value(0);
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
        if (loop_target_end_time - time_now > 16380) // delayMicroseconds is limited to 16383 so use delay() instead if longer than this
        { delay( (loop_target_end_time - time_now )/1000 );}
        else{delayMicroseconds(loop_target_end_time - time_now); }
    }
    else
    { // couldn't keep up
        #ifdef SERIAL_DEBUG_PRINTING
            Serial.println("can't keep up");
        #endif
        loop_target_end_time = time_now+ LOOP_TIME_US;
    }
    measured_velocity_ticks_per_timestep = get_measured_velocity_from_encoder(); // this should happen as soon as possible after the delay

  // check for new demand value:
  if(new_control_register_value_received_flag){
    new_control_register_value_received_flag=false;
    #ifdef STEP_INPUT_TEST
      logging_running=true;
    #endif
    interpret_control_register_value();
  }

  int raw_motor_power_value;
  // depending on motor_state, decide what to do:
  if (motor_state == STANDBY){set_motor_power_value(0);}
  else { // must be FORWARD, REVERSE, or BRAKE (brake is treated as a target velocity of zero, so controller still applied)
        raw_motor_power_value = controller(demand_velocity, measured_velocity_ticks_per_timestep);
        set_motor_power_value(raw_motor_power_value);
    }

  #ifdef STEP_INPUT_TEST   // Data logging:
        if (logging_running){
            data_log[i] = measured_velocity_ticks_per_timestep; // save value
            data_log_inputs[i] = raw_motor_power_value;
            i++;
            if (i>=N_POINTS_TO_LOG){
                logging_running = false;
                for (i = 0; i < N_POINTS_TO_LOG; i++) {
                    Serial.print(data_log_inputs[i]);
                    Serial.print(",");
                }
                Serial.println();
                for (i = 0; i < N_POINTS_TO_LOG; i++) {
                    Serial.print(data_log[i]);
                    Serial.print(",");
                }
                Serial.println();
                i=0;
            }
        }
    #endif
}
