// RF Transmiter to talk to the gripper
#include <SPI.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <RF24.h>

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);

unsigned int val_to_send_a = 0;
unsigned int val_to_send_b = 0;

byte addresses[][6] = {"1Node", "2Node"}; // first for gripper, second for Assembly Fixture

void setup() {
  Serial.begin(9600);
  //Serial.println(F("RF transmitter test"));

  radio.begin();

  // Setting the power level to low, prevents issues using the arduino power supply, but will reduce range
  // options are RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  radio.setPALevel(RF24_PA_MAX);


  Serial.println("I am the transmitter");

}

void loop() {

  // ******************************** Serial part ********************************

  // We expect a specific order of things, for a gripper setting:
  // - the char "a"
  // - the value for gripper A
  // - the char "b"
  // - the value for gripper B
  // anyhting else will not work and be ignored
  // Will send "ready" as a signal to send next set
  // Will send "error" when things don't work

  Serial.println("ready");
  bool send_to_gripper =false;
  bool send_to_AF = false;
  while (Serial.available() == 0) {} // wait for new values from PC via serial:
  char serial_input = Serial.read();
  bool serialReadSuccess = false;
  if (serial_input == 'a') // first char expected is "a" for gripper
  {
    while (Serial.available() == 0) {} // wait for data
    val_to_send_a = int(Serial.parseInt()); // read an integer value

    while (Serial.available() == 0) {} // wait for data
    char serial_input = Serial.read();
    if (serial_input == 'b') // next char expected is "b"
    {
      while (Serial.available() == 0) {} // wait for data
      val_to_send_b = int(Serial.parseInt()); // read an integer value
      serialReadSuccess = true;
      send_to_gripper = true;
    }
  }
//  else if (serial_input == 'c') // send to AF
//  {
//    while (Serial.available() == 0) {} // wait for data
//    val_to_send_AF = int(Serial.parseInt()); // read an integer value
//    serialReadSuccess = true;
//    send_to_AF = true;
//  }

  if (!serialReadSuccess) {
    if (!isspace(serial_input) && serial_input != '\n' && serial_input != '\r') { // ignore spaces
      Serial.println("error");
      Serial.print("Serial input was: ");
      Serial.println(serial_input);
      while (Serial.available()) {
        Serial.read(); // clear buffer
      }
    }
  }



  if (send_to_gripper) {
    send_to_gripper = false;
    // compute val to send over RF:
    unsigned long val = ((unsigned long)val_to_send_a) << 16 | val_to_send_b;
    radio.openWritingPipe(addresses[0]);
    if (!radio.write( &val, sizeof(unsigned long) )) {
       Serial.println(F("error"));
       Serial.println(F("failed to send"));
      }else{
        Serial.println(F("success"));
      }
    
  } 
//  else if (send_to_AF) {
//    send_to_AF = false;
//    radio.openWritingPipe(addresses[1]);
//    if (!radio.write( &val_to_send_AF, sizeof(unsigned int) )) {
//       Serial.println(F("error"));
//       Serial.println(F("failed to send"));
//      }else{
//        Serial.println(F("success"));
//      }
//  }

}
