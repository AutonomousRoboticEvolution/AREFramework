// Firmware for the wheel organ, with encoder. Authors: Matt, Mike
// This pin assignments are the same as for the arduino Uno
//#define SERIAL_DEBUG_PRINTING
//#define OPEN_LOOP_VALUES_FINDER
//#define STEP_INPUT_TEST
#define INDICATOR_LED_SHOW_MOTOR_POWER
#define DEBUG 0

#define SLAVE_ADDRESS 0x60 // <=== THIS NEEDS TO BE SET FOR EACH UNIQUE ORGAN

#include <Wire.h>

// This code should interface with Mike's driver code, and so mimics the register(s) used by the i2c motor driver (DRV8830):
#define DRV_CONTROL_REG_ADDR 0x00
#define DRV_FAULT_REG_ADDR 0x01 // not (yet?) implemented

#define SET_TEST_VALUE_REGISTER 0x90 // save a given value
#define GET_TEST_VALUE_REGISTER 0x91 // return the saved value

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
#define PIN_MOTOR_ENABLE 14

//CURRENT LIMITER DEFAULTS
#define DIG_POT_ADDRESS 0x2E //7-bit address of programmable resistor. Wire library uses 7 bit addressing throughout
#define MIN_CURRENT_MA 45 //milliamps
#define MAX_CURRENT_MA 990 //milliamps
#define DEFAULT_CURRENT_LIMIT MAX_CURRENT_MA

// define controller settings:
#define LOOP_TIME_US 10000 // will set the controller update frequency; 10,000us -> 100Hz

//Slew rate limiting parameter
//Total output range is -255 to +255 (510 total), so e.g. a value of 10 here would take 51 timesteps to do a maximum end-end swing
//For a loop time of 10ms this example would be 510ms
//A rate of 125 of more is too fast (motor cuts out).
#define MAX_OUTPUT_SLEW_RATE 50 //Motor power demand output cannot change by more than this per timestep.

int last_output_power = 0; //store last demanded power from motor for slew rate control

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
int current_limit_in_milliamps;

uint8_t test_register_value = 0;
bool test_resgister_is_requested = false;

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


void setup(){
    // set the pin modes
    pinMode(PIN_MOTOR_DIR_PIN, OUTPUT);
    pinMode(PIN_MOTOR_PWM_PIN, OUTPUT);
    pinMode(PIN_INDICATOR_LED, OUTPUT);
    pinMode(PIN_I2CENABLE, OUTPUT);
    pinMode(PIN_MOTOR_ENABLE, OUTPUT);

    //encoder start
    attachInterrupt(PIN_ENCODER_A, A_rises, RISING);
    attachInterrupt(PIN_ENCODER_B, B_rises, RISING);

    // start i2c comms
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);

    //Enable motor
    motorEnable(true);

    //Set current limit to default value (scaled to tens of mA)
    setCurrentLimit(DEFAULT_CURRENT_LIMIT/10);

    //Open I2C channel to outside world
    extI2CEnable(true);

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

    //Start serial and report existence
    Serial.begin(115200); // for debugging only
    Serial.println("I am a wheel organ...");
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

    case SET_TEST_VALUE_REGISTER:
      test_register_value = input_buffer[1];
        #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("new test_register_value: 0x");
        Serial.print(test_register_value, HEX);
        Serial.print("\t= ");
        Serial.println(test_register_value);
      #endif
      break;
    
    case GET_TEST_VALUE_REGISTER:
      test_resgister_is_requested = true;
      break;
  
    default:
      #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("Attempt to access unknown register: 0x");
        Serial.println(input_buffer[0], HEX);
      #endif
      break;
  }
}

// gets called whenever master wants to read a register:
void sendData(){
  if (test_resgister_is_requested){
    test_resgister_is_requested=false;
    Wire.write(test_register_value);
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print("test register value us requested, sent: 0x");
      Serial.print(test_register_value,HEX);
      Serial.print("\t= ");
      Serial.println(test_register_value);
    #endif
    
  }else{
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.println("ERROR! in sendData, but no request flag is true");
    #endif
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

void set_motor_power_value(int target_power){

    //Check the magnitude of the demanded change in output
    int change_magnitude = absDiff(target_power, last_output_power);
    int output_power_set;

    //If target value is too far from previous value, constrain the magnitude of the change to restrict the slew rate
    if (change_magnitude > MAX_OUTPUT_SLEW_RATE) {
        //Add (or subtract, for negative the maximum increment from the last output power if demanded slew rate is too fast
        output_power_set = (target_power > last_output_power) ? (last_output_power + MAX_OUTPUT_SLEW_RATE) : (last_output_power - MAX_OUTPUT_SLEW_RATE);
    } else {
        //Otherwise can go straight to the full target power
        output_power_set = target_power;
    }

    //Set the motor output to the target value
    if (output_power_set<0){
        // negative
        digitalWrite(PIN_MOTOR_DIR_PIN,LOW);
        analogWrite(PIN_MOTOR_PWM_PIN,-output_power_set);
    }
    else if (output_power_set>0){
        // positive
        digitalWrite(PIN_MOTOR_DIR_PIN,HIGH);
        analogWrite(PIN_MOTOR_PWM_PIN,output_power_set);
    } else { //zero
        digitalWrite(PIN_MOTOR_PWM_PIN,0);
    }

    //Update the global variable for the last output value
    last_output_power = output_power_set;

    //Display power on indicator LED
    #ifdef INDICATOR_LED_SHOW_MOTOR_POWER
        analogWrite(PIN_INDICATOR_LED, abs(output_power_set));
    #endif
}

/*
    absDiff
    @brief Finds the absolute difference between two signed integers
    @param val1 The first integer
    @param val2 The second integer
    @return The magnitude of the difference between the two integers
*/
int absDiff(int val1, int val2) {
    //If val1 is greater than val2 (or equal)
    //Both positive: v1-v2 = +ve
    //Both negative but val1 smaller: v1-v2 = +ve
    //Val1 positive and val2 negative: v1-v2 = +ve
    if (val1 >= val2) {
        return val1-val2;
    } else {
        //If val2 is greater than val1
        //Same three possibilities as above, all give +ve results
        return val2-val1;
    }
}

/*
  motorEnable
  @brief Enables/disables power to the motor via current limiter chip
  @param on_not_off TRUE to switch on, FALSE to switch off
*/
void motorEnable(bool on_not_off) {
  if (on_not_off) {
    digitalWrite(PIN_MOTOR_ENABLE, HIGH);
  } else {
    digitalWrite(PIN_MOTOR_ENABLE, LOW);
  }
}

/*
  extI2CEnable
  @brief Enables/disables external I2C bus connection via repeater chip
  @param on_not_off TRUE to switch on, FALSE to switch off
*/
void extI2CEnable(bool on_not_off) {
  if (on_not_off) {
    digitalWrite(PIN_I2CENABLE, LOW); //Active-low pin
  } else {
    digitalWrite(PIN_I2CENABLE, HIGH);
  }
}

/*
  setCurrentLimit
  @brief Sets the current limit for the motor.
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
  //Rtotal = 100K*(pot_value/127) + 4.53K
  //Rearranging gives pot_value = (127/100)*(4500/current_limit_mA - 4.53)
  uint8_t pot_value = 1.27*((float)4500/current_limit_mA - 4.53);

  //Write to digital potentiometer
  //Must isolate from external I2C bus first to not interact with other joints
  extI2CEnable(false);
  Wire.beginTransmission(DIG_POT_ADDRESS); // transmit to device
  Wire.write(pot_value);            // set register value
  int tx_info = Wire.endTransmission();     // stop transmitting 
  extI2CEnable(true);

  //Update global variable for current limit
  current_limit_in_milliamps = current_limit_mA;

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
