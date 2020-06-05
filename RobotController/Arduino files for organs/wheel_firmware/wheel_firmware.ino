// Firmware for the wheel organ, with encoder. Authour: Matt
// This code is written for an Arduino Nano

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
#define PIN_DRIVER_AIN1 8
#define PIN_DRIVER_AIN2 7
#define PIN_DRIVER_APWM 6
#define PIN_DRIVER_ENABLE 9

// define controller settings:
#define LOOP_TIME_US 10000 // will set the controller update frequency; 10,000us -> 100Hz

// flags and global variales:
bool is_enabled = false;
bool new_control_register_value_received_flag = false;
uint_8 raw_input_for_control_register;
int motor_power;
unsigned long last_loop_time;
int motor_state; //will be either STANDBY, REVERSE, FORWARD or BRAKE defined above
int demand_velocity;


// from the raw 8-bit input value, extract the motor state and demand velocity values
void interpret_control_register_value(){
  // extract motor state bits:
  motor_state = raw_input_for_control_register & 0x03; // last 2 bits
  // extract the motor speed bits:
  unsigned uint8_t speed_value_raw = raw_input_for_control_register & 0xFC >> 2; // first 7 bits
  
  if (motor_state == FORWARD) { demand_velocity = speed_value_raw; }
  else if (motor_state == REVERSE) { demand_velocity = -speed_value_raw; }
  else { demand_velocity = 0; } // either BRAKE or STANDBY

  // debugging output:
  Serial.println("New motor state and demand velocity are:");
  Serial.println(motor_state);
  Serial.println(demand_velocity);
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
  //debugging:
  //Serial.print("Received: 0x");
  //Serial.print(input_buffer[0],HEX);
  //Serial.print(", 0x");
  //Serial.println(input_buffer[1],HEX);
  
  switch (input_buffer[0]) {
    case DRV_CONTROL_REG_ADDR: // set control register
      raw_input_for_control_register = input_buffer[1];
      _flag = true;
      break;
  
    default:
      Serial.print("Attempt to access unkown register: 0x");
      Serial.println(input_buffer[0], HEX);
      break;
  }
}     

int get_measured_velocity_from_encoder(){
  // to do!
  return 0;
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
    enable_motor()
    digitalWrite(PIN_DRIVER_AIN1,HIGH);
    digitalWrite(PIN_DRIVER_AIN2,HIGH);
    digitalWrite(PIN_DRIVER_APWM,0);
}

// will return the raw motor power value, in the range -255 to +255
int controller( demand_velocity, measured_velocity ){
    // to do properly, for now just output proportional to demand value
    return demand_velocity * 4;
}

void set_motor_power_value(value){
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
  pinMode(PIN_DRIVER_AIN1, OUTPUT); 
  pinMode(PIN_DRIVER_AIN2, OUTPUT); 
  pinMode(PIN_DRIVER_APWM OUTPUT); 
  pinMode(PIN_DRIVER_ENABLE, OUTPUT);
  
  digitalWrite(PIN_DRIVER_ENABLE, LOW); // start not enabled
  
  Serial.begin(115200); // for debugging only
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  
  last_loop_time = micros();
}

void loop(){
  // wait until it is time for next loop
  delayMicroseconds(last_loop_time + LOOP_TIME_US - micros() );
  last_loop_time += LOOP_TIME_US;
  int measured_velocity = get_measured_velocity_from_encoder(); // this should happen as soon as possible after the delay, because it assumes the correct time interval between calls

  Serial.println(micros());// debug
  
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
      int raw_motor_power_value = controller(demand_velocity, measured_velocity);
      set_motor_power_value(raw_motor_power_value);
  }
}
