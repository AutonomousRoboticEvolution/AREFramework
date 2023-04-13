/* Firmware to be run on the microcontroller in the sensor organ
 *  
 *  From the pi (master) point of view, the i2c registers available are:
 *  0x02: request infrared reading. This returns the latest value of the beacon detection system (described below).
 *  0x03: int8: flash the indicator LED this number of times (then return to showing the IR value)
 *  0x04: int8: set the VL53L0X time-of-flight (TOF) sensor address
 *  0x05: request the curent value of the VL53L0X address
 *  0x06: request a raw reading from the IR sensor, without applying any filtering etc (returns a 2 byte value)
 *  0x07: set the threashold for the binary value for IR sensor. At the moment, this only matters for the binary indicator LED
 *  0x90: set the test register. Will just save the given value, to be returned by get test register value
 *  0x91: get the previously set test register value
 *  
 *  The IR reading returned is the calculated using the following process:
 *  (1) take a new sample every SAMPLE_PERIOD_US, untill you have taken BUFFER_SIZE samples.
 *  (2) Apply a window of WINDOW_SIZE to the samples, and for each window find the maximum minus minimum value to get an estimate for the peak-to-trough value
 *  (3) Since interference will (probably) only ever increase this peak-to-trough value, take the minimum peak-to-trough value found over the entire sample.
 *  
 *  SAMPLE_PERIOD_US value should be chosen such that the samples are taken several times faster than the beacon flashing frequency. 
 *  The overall time that samples are taken over (i.e. SAMPLE_PERIOD_US*BUFFER_SIZE) should be long enough to avoid the TOF interference covering the entire sample,
 *  but this period dictates the update rate of IR sensor values so it should not be too long.
 *  Additionally, if BUFFER_SIZE is too big, the Arduino may run out of memory (the buffer is this many 16-bit values).
 *  WINDOW_SIZE should be chosen with reference to SAMPLE_PERIOD_US and the beacon flashing frequency (600Hz) so that the window contains one complete waveform of the beacon signal
 *  e.g., if SAMPLE_PERIOD_US = 167 microseconds that gives a sampling frequency of 6000Hz, which is 10 times faster than the beacon so the WINDOW_SIZE should be 10
 * 
 * Author: Mike Angus, Matt Hale
*/

#include <Wire.h>
#include <VL53L0X.h>
VL53L0X timeOfFlightSensor;

#define SLAVE_ADDRESS 0x30 // <=== THIS NEEDS TO BE SET FOR EACH UNIQUE ORGAN

// debugging flags:
//#define SERIAL_DEBUG_PRINTING
//#define SERIAL_DEBUG_TOF_SETUP
//#define SERIAL_DEBUG_PRINT_IR_READING_VALUE // prints the computed IR reading (after all filtering etc) every time it is computed (~3 times per second)
//#define SERIAL_DEBUG_DETAILED_IR_VALUES // prints the entire sample of raw values, and pauses ~2 seconds between each reading. Don't leave this on!


// define register addresses this slave device provides
#define REQUEST_INFRARED_REGISTER 0x02
#define FLASH_INDICATOR_LED_REGISTER 0x03
#define SET_TIME_OF_FLIGHT_ADDRESS_REGISTER 0x04 // set the i2c address of the VL53L0X sensor
#define GET_TIME_OF_FLIGHT_ADDRESS_REGISTER 0x05 // return what the arduino thinks is the i2c address of the VL53L0X sensor
#define REQUEST_INFRARED_RAW_VALUE_REGISTER 0x06 // return the raw IR value, without the filtering
#define SET_INFRARED_THREASHOLD_REGISTER 0x07 // set the threashold for IR sensor
#define SET_TEST_VALUE_REGISTER 0x90 // save a given value
#define GET_TEST_VALUE_REGISTER 0x91 // return the saved value

// define pins:
#define INDICATOR_LED_PIN 9
#define IR_MEASURE_PIN 7
#define PIN_I2CENABLE 15

// define sensor filtering parameters
#define BUFFER_SIZE 400 //200
#define SAMPLE_PERIOD_US 167// 167 microseconds -> 6000Hz
#define WINDOW_SIZE 10 // make this the beacon 

//define other constants
#define LED_FLASH_HALF_TIMEPERIOD 500 // the time between the LED coming on and going off (and vice versa) when flashes are requested, in milliseconds
#define TIME_OF_FLIGHT_SENSOR_HARDWARE_DEFAULT_ADDRESS 0x29
#define VL53L0X_CHANGE_ADDRESS_REGISTER 0x8A // VL53L0X Register for change i2c address, from https://community.st.com/s/question/0D50X00009XkYG8/vl53l0x-register-map

// flags and global variables:

// used for general i2c comms
uint8_t input_buffer[2];
uint8_t send_buffer;
bool first_byte_has_been_sent=false; // used when sending two-byte replies to keep track of whether we've sent the first one already

// used for IR sensor
uint16_t raw_values[BUFFER_SIZE];
volatile uint8_t reading; // variable to store the IR reading after all the processing.
uint16_t raw_reading; // used only for REQUEST_INFRARED_RAW_VALUE_REGISTER
uint8_t IR_threashold=0;

// things used for controlling the indicator LED
unsigned int number_of_flashes_requested = 0;
unsigned long time_of_previous_led_flash =0;
bool led_is_on = false;
bool set_LED_to_IR_value = true;

uint8_t test_register_value = 0;

// for controlling the TOF sensor. Note the actual readings are read by the pi directly from the TOF, but this code will set the i2c address.
bool new_address_for_time_of_flight_sensor_received = false;
int new_address_of_time_of_flight_sensor = TIME_OF_FLIGHT_SENSOR_HARDWARE_DEFAULT_ADDRESS;
int current_address_of_time_of_flight_sensor = TIME_OF_FLIGHT_SENSOR_HARDWARE_DEFAULT_ADDRESS; // don't worry, this gets chenged in setup()



/***** Setup function *****/
void setup() {
  // set the pin modes
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN,LOW);
  pinMode(PIN_I2CENABLE, OUTPUT);
  digitalWrite(PIN_I2CENABLE, HIGH); // close i2c to outside world

  Serial.begin(115200); //formerly 2000000
  Serial.print("Sensor Organ Reporting; my i2c address is 0x");
  Serial.println(SLAVE_ADDRESS,HEX);
  
  // start i2c comms
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  
  // set up the time of flight sensor:
  timeOfFlightSensor.setTimeout(500);
  #ifdef SERIAL_DEBUG_TOF_SETUP
    Serial.println("Setting up timeOfFlightSensor");
  #endif
  if (!timeOfFlightSensor.init())
  {
    // try again, but this time assume that the TOF has alread had its address changed (e.g. after Arduino browns out, but TOF sensor has not):
    timeOfFlightSensor.setAddress(SLAVE_ADDRESS+1);
      if (!timeOfFlightSensor.init())
      { // now I'm out of ideas
        #ifdef SERIAL_DEBUG_TOF_SETUP
          Serial.println("Failed to detect and initialize VL53L0X sensor!");
        #endif
      }else{
        #ifdef SERIAL_DEBUG_TOF_SETUP
          Serial.println("VL53L0X sensor already had its new adress at initialization (OK)");
        #endif
      }
  }

  // set to "long range":
  timeOfFlightSensor.setSignalRateLimit(0.1); // lower the return signal rate limit (default is 0.25 MCPS)
  timeOfFlightSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);// increase laser pulse periods (defaults are 14 and 10 PCLKs)
  timeOfFlightSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

  
  timeOfFlightSensor.startContinuous(80); // the input here is the time between readings in milliseconds. Set smaller to get more up-to-date TOF values, set bigger to reduce interference with IR sensors.
  timeOfFlightSensor.setAddress(SLAVE_ADDRESS+1);
  digitalWrite(PIN_I2CENABLE, LOW); // open i2c to outside world

  Serial.println("Starting");
  
} // end of setup()


//Main loop
void loop() {

  getBeaconLevel(); // updates reading

  
  // is it time to turn the LED on or off?
  unsigned long time_now=millis();
  if (number_of_flashes_requested > 0 && time_of_previous_led_flash<time_now - LED_FLASH_HALF_TIMEPERIOD){
    if(led_is_on){
      digitalWrite(INDICATOR_LED_PIN,LOW);
      led_is_on=false;
      number_of_flashes_requested -= 1; // decrement after turning off so we always end in the off state
      if (number_of_flashes_requested==0) set_LED_to_IR_value=true; // finished flashing, so go back to indicating the IR level
    }else{
      set_LED_to_IR_value = false;
      digitalWrite(INDICATOR_LED_PIN,HIGH);
      led_is_on=true;
    }
    time_of_previous_led_flash = time_now;
  }

  if (new_address_for_time_of_flight_sensor_received){
    new_address_for_time_of_flight_sensor_received=false;
    timeOfFlightSensor.setAddress(new_address_of_time_of_flight_sensor & 0x7F);
  }
}
