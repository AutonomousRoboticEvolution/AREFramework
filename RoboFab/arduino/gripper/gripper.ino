// gripper arduino firmware - receives instructions over the serial/RS485 connection, and sets the servo values


#include <SPI.h>
#include <Servo.h>

// this must be disabled for use with the python script, but is useful for debugging when using the manual serial console:
const bool DEBUG_PRINTING = false;

// pin definitions:
const int green_led_pin = A0;
const int red_led_pin = A2;
const int RS485_wirte_activation_pin = 8;
const int servoPinA = 9;
const int servoPinB = 10;

Servo servoA;
Servo servoB;


void setup() {

  pinMode(green_led_pin, OUTPUT);
  pinMode(red_led_pin, OUTPUT);
  pinMode(RS485_wirte_activation_pin, OUTPUT);

  
  digitalWrite(green_led_pin, 1); digitalWrite(red_led_pin, 1); delay(200); digitalWrite(green_led_pin, 0); digitalWrite(red_led_pin, 0); // flash both leds

  // start serial and go through "handshake"
  Serial.begin(9600);
  digitalWrite(RS485_wirte_activation_pin, 0); // listening mode
  //while (Serial.available() == 0) {} // wait for start message (anything)
  while (Serial.available() != 0) {Serial.read();} // flush serial port
  
  digitalWrite(green_led_pin, 1); delay(200); digitalWrite(green_led_pin, 0); // flash green for good!
  digitalWrite(RS485_wirte_activation_pin, 1); // writing mode
  Serial.println("I am the gripper");
  if (DEBUG_PRINTING){Serial.println("debug printing is on");}
  digitalWrite(RS485_wirte_activation_pin, 0); // listening mode

  // set up servos:
  servoA.attach(servoPinA);
  servoB.attach(servoPinB);
  servoA.writeMicroseconds(1500);
  servoB.writeMicroseconds(1500);
  delay(500); // allow servos to move to centre positions, then set to zero to stop annoying noise
  servoA.writeMicroseconds(0);
  servoB.writeMicroseconds(0);

}



void loop() {

  // ******************************** receive part ********************************
  // for the servo microsecond (typically 1000-2000) signal values:
  int valA;
  int valB;
  
  if (Serial.available() > 0) {
    while (Serial.available() == 0) {} // wait for new values from PC via serial:
    char serial_input = Serial.read();
    bool serialReadSuccess = false;
    if (serial_input == 'a') // first char expected is "a" for gripper
    {
      while (Serial.available() == 0) {} // wait for data
      valA = int(Serial.parseInt()); // read an integer value
  
      while (Serial.available() == 0) {} // wait for data
      char serial_input = Serial.read();
      if (serial_input == 'b') // next char expected is "b"
      {
        while (Serial.available() == 0) {} // wait for data
        valB = int(Serial.parseInt()); // read an integer value
        serialReadSuccess = true;
      }
    }
  
    // ******************************** actuate servo ********************************
    if (serialReadSuccess){
      servoA.writeMicroseconds( valA );
      servoB.writeMicroseconds( valB );
      digitalWrite(green_led_pin, 1); delay(200); digitalWrite(green_led_pin, 0); // flash green for good
    }else{
      digitalWrite(red_led_pin, 1); delay(200); digitalWrite(red_led_pin, 0); // flash red for bad
    }

    // ******************************** report back to PC ********************************
    digitalWrite(RS485_wirte_activation_pin, 1); // writing mode
    if (serialReadSuccess){
      Serial.println("OK");
      if (DEBUG_PRINTING){
        Serial.println(valA);
        Serial.println(valB);
      }
    }
    else{ // not serialReadSuccess
      Serial.println("error");
    }
    digitalWrite(RS485_wirte_activation_pin, 0); // listening mode

    while (Serial.available() != 0) {Serial.read();} // flush serial port
  }
  
  else { // no message available
    delay(100);
  }
}
