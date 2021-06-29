/*
* Author = Matt
* Date = May 2020
* Description; This code controls the joint organ (based of Rob's original i2c_servo code)
*/

// calibration values for this particular servo:
#define zero_degree_us 400
#define size_of_us_range 1950 // the difference between the zero and 180degree positions
#define SLAVE_ADDRESS 0x07

#include <Wire.h>
#include <Servo.h>
Servo PWMservo;

//Registers implemented:
#define TARGET_POSTIION_REGISTER 0x10
#define JOINT_MODE_REGISTER 0X11
#define MEASURED_POSITION_REGISTER 0x12

//the options for JOINT_MODE_REGISTER
#define SERVO_OFF 0x00
#define SERVO_ON 0x01

// define pins
#define SERVO_POSITION_PIN A0
#define SERVO_PWM_PIN 4

//TESTCODE
#define I2CENABLE 15

// we need a buffer for an 8-bit register, and another 8-bit byte for the actual value of received data:
uint8_t input_buffer[2];
uint8_t send_buffer;
//bool received_data_flag = false;
//int received_data_byte_count;
bool measured_joint_position_is_up_to_date = false;

// global variables
uint8_t current_target_joint_position; // this is 0-255, representing angles over approx 180degrees
uint8_t mode; // either SERVO_ON or SERVO_OFF
uint8_t measured_joint_position;
bool servo_is_started = false;

void setup(){
  pinMode(SERVO_POSITION_PIN,INPUT);
  
  /// Begins serial with pc
  Serial.begin(115200); // start serial for output
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("I2C Ready!");

  //TESTCODE Open the I2C channel to the outside world (active-low)
  pinMode(I2CENABLE, OUTPUT);
  digitalWrite(I2CENABLE, LOW);
  Serial.println("I2C to outside world enabled");
}

void loop(){
  delay(100);
}

// triggered on every conversation:
void receiveData(int received_data_byte_count){

  // the first byte received is the register value, and determines what we should do
  input_buffer[0] = Wire.read(); // register
  if (received_data_byte_count>1){
    input_buffer[1] = Wire.read(); // value
  }else{
    input_buffer[1] = 0;
  }

  Serial.print("Received: 0x");
  Serial.print(input_buffer[0],HEX);
  Serial.print(", 0x");
  Serial.println(input_buffer[1],HEX);
  
  switch (input_buffer[0]) {
    case TARGET_POSTIION_REGISTER: // set servo value
      current_target_joint_position = input_buffer[1];
      set_servo(current_target_joint_position);
      break;
  
    case JOINT_MODE_REGISTER: // turn joint on or off
      mode = input_buffer[1];
      //Serial.println(mode,HEX);
      if (mode&0x01 == SERVO_ON){ 
        Serial.println("Turn servo on");
        set_servo(current_target_joint_position); 
        }
      else { // SERVO_OFF
        Serial.println("Turn servo off");
        if (servo_is_started){
          PWMservo.detach();
          servo_is_started = false;}
      }
      break;
  
    case MEASURED_POSITION_REGISTER:
      measured_joint_position = analogRead(SERVO_POSITION_PIN)/4;
      send_buffer = measured_joint_position;
      measured_joint_position_is_up_to_date = true;
      Serial.print("Preparing to send measured position of: ");
      Serial.print(measured_joint_position);
      Serial.print(" (Hex: ");
      Serial.print(measured_joint_position,HEX);
      Serial.println(")");
      break;

    case 0x99:
      // test register
      send_buffer = 0x99;
      Serial.println("0x99 test");
      break;
          
    default:
      Serial.print("Attempt to access unkown register: 0x");
      Serial.println(input_buffer[0], HEX);
      break;
  }
}



// master wants to read a register:
void sendData(){
  Wire.write(send_buffer);
  measured_joint_position_is_up_to_date = false;
  Serial.println("Sent");
}

// send the servo to a certain position. Input: a value from 0-255, corespoinding to the full 180 degree range of the joint.
void set_servo(uint8_t target_position){
  if (!servo_is_started){
    PWMservo.attach(SERVO_PWM_PIN);
    servo_is_started = true;
  }
        
  unsigned int us_value = zero_degree_us +  size_of_us_range* float(target_position/255.0);
  PWMservo.writeMicroseconds(us_value);
  
  Serial.print("I've been told to set the position to: ");
  Serial.print(target_position,DEC);
  Serial.print(", which is a microsecond value of:");
  Serial.println(us_value,DEC);
}
