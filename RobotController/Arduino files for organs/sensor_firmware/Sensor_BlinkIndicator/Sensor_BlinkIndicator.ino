/*

*/
#include <Wire.h>

#define INDICATOR 9
#define I2CENABLE 15

#define FLASHDELAY 200
#define SLAVE_ADDRESS 0x07

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(INDICATOR, OUTPUT);
  pinMode(I2CENABLE, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sensor Organ Reporting");

  //Open the I2C channel to the outside world (active-low)
  digitalWrite(I2CENABLE, LOW);
  //I2C slave setup
  Wire.begin(SLAVE_ADDRESS);
  Serial.println("I2C to outside world enabled");
}

// the loop function runs over and over again forever
void loop() {

//int reading = analogRead(IR_MEASURE_PIN);
//Serial.print("IR Reading: ");
//Serial.println(reading);

 //LED Flashing Routine
  digitalWrite(INDICATOR, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.println("FLASH ON");
  delay(FLASHDELAY);                       // wait for a moment
  digitalWrite(INDICATOR, LOW);    // turn the LED off by making the voltage LOW
  Serial.println("FLASH OFF");
  delay(FLASHDELAY);                       // wait for a moment
}
