/*
 * Author = Matt (based of Rob's original i2c_servo code)
 * Date = May 2020
 * Description; This code controls a Pico microcontroller. It is design to recieve I2C messages and sent a PWM signal to a analogue servo MG996R - Digi Hi Torque.
 */

const int zero_degree_us = 400;
const int oneeighty_degree_us = 2350;

/// i2c_slave_test.ino
#include <Wire.h>
#include <Servo.h>                           // Include servo library
 
// standard Arduino library
Servo PWMservo;  

// ---- NEEDS TO BE CHANGED FOR EACH SERVO ----
// defines the address for the servo  
#define SLAVE_ADDRESS 0x07

// 10 byte data buffer
int receiveBuffer[9];
// count for debug protocol
uint8_t keepCounted = 0;

void set_servo(float angle_degrees){
  int us_value = zero_degree_us +  (oneeighty_degree_us-zero_degree_us)*(angle_degrees/180.0);
  PWMservo.writeMicroseconds(us_value);
}

/// Read data in to buffer, offset in first element.
void receiveData(int byteCount){
  // buffer count to place message in different memory
  int counter = 0;
  while(Wire.available()) {
    // writes the data to the buffer
    receiveBuffer[counter] = Wire.read();
    // print some data for debugging to serial
    Serial.print("Got data: ");
    Serial.println(receiveBuffer[counter]);
    counter ++;
  }
  // A message to move the servo to a given angle
  if (receiveBuffer[0] == 0){
    Serial.println("Move Servo");
    int servo_degrees = receiveBuffer[1] * 5;
    set_servo(servo_degrees);
  }
}


/// Use the offset value to select a function
void sendData(){
  if (receiveBuffer[0] == 99) {
    writeData(keepCount());
  } else {
    Serial.println("No function for this address");
  }
}


/// Write data
void writeData(char newData) {
  // Sents a message to the i2c master
  uint8_t data[] = {receiveBuffer[0], newData};
  int dataSize = sizeof(data);
  Wire.write(data, dataSize);
}


/// Counter function for debugging and connection/reset check
int keepCount() {
  keepCounted ++;
  if (keepCounted > 255) {
    keepCounted = 0;
    return 0;
  } else {
    return keepCounted;
  }
}


void setup(){
  /// Begins serial with pc
  Serial.begin(9600); // start serial for output
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("I2C Ready!");
  
  //Attach servo to D4
  PWMservo.attach(4);                      // Attach left signal to pin D4
  // Sets starting position to the middle, i.e. straight joint:
  set_servo(90);
}


void loop(){
  // waits for a i2c message
  delay(100);
}
