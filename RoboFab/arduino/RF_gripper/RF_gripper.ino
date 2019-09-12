// RF gripper - receives instructions from the trasmitter, and sets the servo values


#include <SPI.h>
#include <RF24_config.h>
#include <nRF24L01.h>
//#include <printf.h>
#include <RF24.h>
#include <Servo.h>



/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);

int servoPinA = 5;
int servoPinB = 6;
Servo servoA;
Servo servoB;

byte address[6] = {"1Node"};


void setup() {
  Serial.begin(9600);
  Serial.println(F("Remote gripper test"));

  radio.begin();

  // Setting the power level to low, prevents issues using the arduino power supply, but will reduce range
  // options are RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  radio.setPALevel(RF24_PA_LOW);

  // Open a reading pipe to get messages
  radio.openReadingPipe(1, address);
  Serial.println("I am the receiver / gripper");

  // Start the radio listening for data
  radio.startListening();


  servoA.attach(servoPinA);
  servoB.attach(servoPinB);
  servoA.writeMicroseconds(1500);
  servoB.writeMicroseconds(1500);
}



void loop() {

  // ******************************** receive part ********************************
  unsigned long val_in;

  if ( radio.available()) {
    while (radio.available()) {                                   // While there is data ready
      radio.read( &val_in, sizeof(unsigned long) );             // Get the payload
    }


    // ******************************** convert to A and B ********************************
    unsigned int valA = val_in >> 16;
    unsigned int valB = val_in & 0xFFFF;


    // ******************************** debug printing ********************************
    Serial.print(F("Got message: "));
    Serial.println(val_in);
    Serial.print(F("Binary: "));
    Serial.println(val_in, BIN);
    Serial.print(F("valA: "));
    Serial.print(valA, BIN);
    Serial.print(F(", "));
    Serial.println(valA);
    Serial.print(F("valB: "));
    Serial.print(valB, BIN);
    Serial.print(F(", "));
    Serial.println(valB);




    // ******************************** actuate servo ********************************
    servoA.writeMicroseconds( valA );
    servoB.writeMicroseconds( valB );

    Serial.println("values sent to servos");
    Serial.println("---");
  }
  else { // no payload avilable
    delay(100);
  }
}
