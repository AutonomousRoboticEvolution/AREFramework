/*
  Authors: Mike Angus, Matt Hale
  Date: Sept 2021
  Description: Firmware for Joint Organ v1.2
*/

//I2C ADDRESS
#define SLAVE_ADDRESS 0x12// <=== THIS NEEDS TO BE SET FOR EACH UNIQUE ORGAN

//CALIBRATION VALUES (replace with copy-paste from joint_calibration output)
#define CALIB_CENTRE_POSITION_US 1440
#define CALIB_ENCODER_VALUE_AT_MIN_ANGLE 123
#define CALIB_ENCODER_VALUE_AT_MAX_ANGLE 887

//DEBUG FLAG
#define DEBUG 0
#define HARDWARE_TEST 0

//CALIBRATION CALCULATIONS
#define DEG_PER_US 0.093011  //Approximately measured empirically based on a travel of ~173 degrees
#define NOMINAL_PWM_CENTRE_US 1470
#define NOMINAL_PWM_MIN_US 540  //Consistent across servos
#define NOMINAL_PWM_MAX_US 2400 //Consistent across servos
#define NOMINAL_PWM_RANGE_US (NOMINAL_PWM_MAX_US - NOMINAL_PWM_MIN_US)
#define SERVO_MOVEMENT_RANGE_DEG 173.0  //Empirically determined. Assumed evenly split around centrepoint (not tested)

#define HALF_SERVO_MOVEMENT_RANGE_DEG (SERVO_MOVEMENT_RANGE_DEG/2)
#define CENTRE_OFFSET_US (CALIB_CENTRE_POSITION_US - NOMINAL_PWM_CENTRE_US)
#define UPPER_PWM_RANGE_US (NOMINAL_PWM_MAX_US - CENTRE_OFFSET_US - NOMINAL_PWM_CENTRE_US)
#define LOWER_PWM_RANGE_US (NOMINAL_PWM_CENTRE_US - NOMINAL_PWM_MIN_US + CENTRE_OFFSET_US)
#define MAX_SERVO_ANGLE (LOWER_PWM_RANGE_US * DEG_PER_US)
#define MIN_SERVO_ANGLE -(UPPER_PWM_RANGE_US * DEG_PER_US)

//CURRENT LIMITER DEFAULTS
#define DIG_POT_ADDRESS 0x2E //7-bit address of device. Wire library uses 7 bit addressing throughout
#define MIN_CURRENT_MA 330 //milliamps
#define MAX_CURRENT_MA 1250 //milliamps
#define DEFAULT_CURRENT_LIMIT 500

#define MAX_SETI_ANALOG_READING 450 //Determined by ADC reference voltage AREF, set by onboard resistor

//Servo position
#define SERVO_POS_READING_MIN 30
#define SERVO_POS_READING_MAX 1000
#define SERVO_POS_READING_RANGE (SERVO_POS_READING_MAX - SERVO_POS_READING_MIN)

//Libraries
#include <Wire.h>
#include <Servo.h>

//I/O pins
#define INDICATOR_LED 6 //This is pin 9 on v1.1 boards, which have no PWM for the LED as it conflicts with the Servo library.
#define I2C_ENABLE 15
#define SERVO_ENABLE 7 //this is pin 6 on v1.1 boards
#define SERVO_PWM 5
#define SETI_PIN 6 //Current measurement pin (Analog pin 6)
#define SERVO_POSITION_PIN A3

//Register addresses
#define SERVO_ENABLE_REGISTER 0X10
#define TARGET_POSITION_REGISTER 0x11
#define MEASURED_POSITION_REGISTER 0x12
#define MEASURED_CURRENT_REGISTER 0x13
#define CURRENT_LIMIT_REGISTER 0x14
#define LED_BRIGHTNESS_REGISTER 0x15
#define SET_TEST_VALUE_REGISTER 0x90 // save a given value
#define GET_TEST_VALUE_REGISTER 0x91 // return the saved value

//Loop rate
#define LOOP_TIME_US 10000 // will set the controller update frequency; 10,000us -> 100Hz

//Slew rate limiting parameter (max allowed value change per timestep)
//Total output range is -127 to +127 (255 total), so e.g. a value of 10 here would take 25 timesteps to do a maximum end-end swing
//For a loop time of 10ms this example would be ~255ms
//Quoted unloaded movement rate for FS5115M-FB servo is 160ms for 60deg at 6V
//Driving at 5V and loaded, so will be slower, but taking this value, would be 480ms for full 180deg swing (48 timesteps)
//This corresponds to a slew rate limit of ~5.3 per timestep. Round up to 6; should be faster than any physical servo dynamics
#define MAX_OUTPUT_SLEW_RATE 3 //Servo position demand output cannot change by more than this per timestep.

//I2C comms
volatile uint8_t input_buffer[2];
volatile uint8_t send_buffer;

//Servo global variables
Servo joint_servo;
bool servo_is_started = false;
int current_limit_in_milliamps;

//Measurements global variables
uint8_t measured_current;
int8_t measured_position;

//Settings global variables
uint8_t current_limit_setting;
int8_t target_position_setting;
uint8_t led_brightness;

//Flags for updating settings
volatile bool update_current_limit_flag = false;
volatile bool update_target_position_flag = false;
volatile bool update_led_brightness_flag = false;

//Loop rate control
unsigned long loop_target_end_time;

//Slew rate limiting global - last demand position
int8_t last_target_position;

//Variables only used for testing
bool rotationDirection = 0; //0 is -ve, 1 is +ve

uint8_t test_register_value = 0; //For organ presence detection

/*******SETUP**********************/
void setup() {
  //Pins setup
  pinMode(INDICATOR_LED, OUTPUT);
  pinMode(SERVO_ENABLE, OUTPUT);
  pinMode(SERVO_PWM, OUTPUT);
  pinMode(I2C_ENABLE, OUTPUT);

  //Connect servo
  joint_servo.attach(SERVO_PWM);

  //Open serial port for debugging
  Serial.begin(115200);
  Serial.println("Joint Organ Reporting");
  Serial.print("My i2c address is 0x");
  Serial.println(SLAVE_ADDRESS,HEX);

  Serial.println("");
  Serial.print("#define CALIB_CENTRE_POSITION_US ");
  Serial.println(CALIB_CENTRE_POSITION_US);
  Serial.print("#define CALIB_ENCODER_VALUE_AT_MIN_ANGLE ");
  Serial.println(CALIB_ENCODER_VALUE_AT_MIN_ANGLE);
  Serial.print("#define CALIB_ENCODER_VALUE_AT_MAX_ANGLE ");
  Serial.println(CALIB_ENCODER_VALUE_AT_MAX_ANGLE);
  Serial.println("");

  //Init I2C bus (internal only at this point)
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("I2C Ready!");

  //Set analog measurement to use external reference
  //MUST be called before any analogRead, to prevent hardware damage
  analogReference(EXTERNAL);

  //Set current limit to default value
  setCurrentLimit(DEFAULT_CURRENT_LIMIT/10);

  //Open I2C channel to outside world
  extI2CEnable(true);
  Serial.println("I2C to outside world enabled");

  // prepare for loop start
  loop_target_end_time = micros();
}

/******MAIN LOOP********************/
void loop() {

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

  //Update measurements
  measured_position = readServoPosition();
  measured_current = readServoCurrent();

  //Show current limit on the indicator LED
  float currentProportionOfLimit = (float)measured_current/MAX_SETI_ANALOG_READING;
  led_brightness = currentProportionOfLimit * 255; //Scale to AnalogOut range
  update_led_brightness_flag = true;

  //Update target position
  setTargetPosition(target_position_setting);

  //Update settings
  if (update_current_limit_flag) {
    setCurrentLimit(current_limit_setting);
    update_current_limit_flag = false;
  }
  
  // if (update_target_position_flag) {
  //   setTargetPosition(target_position_setting);
  //   update_target_position_flag = false;
  // }
  
  if (update_led_brightness_flag){
    update_led_brightness_flag=false;
    setLED(led_brightness);
  }

  //HARDWARE TEST ROUTINE
  if (HARDWARE_TEST) {
    if(rotationDirection == 0) {
      target_position_setting -= 1;
      if(target_position_setting < MIN_SERVO_ANGLE) {
        rotationDirection = 1;
        extI2CEnable(true);
        Serial.println("I2C to outside world enabled");
      } else {
        update_target_position_flag = true;
      }
    } else {
      target_position_setting += 1;
      if(target_position_setting > MAX_SERVO_ANGLE) {
        rotationDirection = 0;
        extI2CEnable(false);
        Serial.println("I2C to outside world disabled");
      } else {
        update_target_position_flag = true;
      }
    }
    Serial.print("Target position: ");
    Serial.println(target_position_setting);
    Serial.print("Current reading: ");
    Serial.print(readServoCurrent());
    Serial.print(" mA, raw: ");
    Serial.println(analogRead(SETI_PIN));
    Serial.print("Position: ");
    Serial.println(readServoPosition());
    delay(10);
  }
}

/*******FUNCTIONS*****************/
/*
  servoEnable
  @brief Enables/disables power to the servo via current limiter chip
  @param on_not_off TRUE to switch on, FALSE to switch off
*/
void servoEnable(bool on_not_off) {
  if (on_not_off) {
    digitalWrite(SERVO_ENABLE, HIGH);
    servo_is_started = true;
  } else {
    digitalWrite(SERVO_ENABLE, LOW);
    servo_is_started = false;
  }
}

/*
  extI2CEnable
  @brief Enables/disables external I2C bus connection via repeater chip
  @param on_not_off TRUE to switch on, FALSE to switch off
*/
void extI2CEnable(bool on_not_off) {
  if (on_not_off) {
    digitalWrite(I2C_ENABLE, LOW); //Active-low pin
  } else {
    digitalWrite(I2C_ENABLE, HIGH);
  }
}

/*
  setCurrentLimit
  @brief Sets the current limit for the joint servo.
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
  //Rtotal = 10K*(pot_value/127) + 3.6K
  //Rearranging gives pot_value = (127/10)*(4500/current_limit_mA - 3.6)
  uint8_t pot_value = 12.7*((float)4500/current_limit_mA - 3.6);

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

/*
  setLED
  @brief Set the PWM of the onboard indicator LED (v1.2+)
  @param pwm_value PWM value from 0-255
*/
void setLED(uint8_t pwm_value) {
  analogWrite(INDICATOR_LED, pwm_value);
}

/*
  readServoCurrent
  @brief Returns the instantaneous current draw of the servo.
  This signal is filtered onboard to smooth the PWM pulses to a continuous current draw value
  @return Current in milliamps (mA)
*/
int readServoCurrent() {
  //Get analog reading from filtered SETI pin on current limiter chip
  //This will be a value from 0 to MAX_SETI_ANALOG_READING
  //Represents the proportion of the set current limit which is being used
  int seti_reading = analogRead(SETI_PIN);

  //Calculate value in milliamps and return it
  float proportion_of_max = (float)seti_reading/MAX_SETI_ANALOG_READING;
  int current_reading = (proportion_of_max * current_limit_in_milliamps);

  //Debug info
  if (DEBUG) {
    Serial.print("Servo current: ");
    Serial.print(current_reading, DEC);
    Serial.println("mA");
  }

 return current_reading;
}

/*
  readServoPosition
  @brief Returns the present position of the servo in range 0-180 degrees
  @return Servo position in degrees
*/
int readServoPosition() {
  //Get analog reading from input pin
  int servo_position = analogRead(SERVO_POSITION_PIN);
  
  //Calculate value in degrees. Note enc. value is at maximum when servo is at min angle, hence reversing of arguments 2 and 3
  int servo_position_degrees = map(servo_position, CALIB_ENCODER_VALUE_AT_MAX_ANGLE, CALIB_ENCODER_VALUE_AT_MIN_ANGLE, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE); //

  //Debug info
  if(DEBUG) {
    Serial.print("Servo Position (raw): ");
    Serial.println(servo_position, DEC);
    Serial.print("Degrees: ");
    Serial.println(servo_position_degrees, DEC);
  }

  return servo_position_degrees;
}

/*
  setTargetPosition
  @brief Set the servo to a target position in degrees.
  The exact PWM is determined from the calibrated microsecond values for each joint.
  @param target_position The target position in degrees -90 to 90. Full range will not actually be available.
*/
void setTargetPosition (int8_t target_position) {
  
  //Limit the slew rate of the demand signal
  int target_position_set;
  int change_magnitude = absDiff(target_position, last_target_position);

  //If target value is too far from previous value, constrain the magnitude of the change to restrict the slew rate
  if (change_magnitude > MAX_OUTPUT_SLEW_RATE) {
      //Add (or subtract, for negative the maximum increment from the last target position if demanded slew rate is too fast
      target_position_set = (target_position > last_target_position) ? (last_target_position + MAX_OUTPUT_SLEW_RATE) : (last_target_position - MAX_OUTPUT_SLEW_RATE);
  } else {
      //Otherwise can go straight to the target position
      target_position_set = target_position;
  }

  //Calculate microseconds value and move servo
  //Note that for any calibrated centrepoint which is not exactly halfway in the PWM range,
  //the servo will not actually be able to reach the full extent in one of the directions
  unsigned int us_value = CALIB_CENTRE_POSITION_US + (float)target_position_set/DEG_PER_US;
  joint_servo.writeMicroseconds(us_value);

  //Update last target position
  last_target_position = target_position_set;

  //Debug info
  if (DEBUG) {
    Serial.print("I've been told to set the position to: ");
    Serial.print(target_position_set,DEC);
    Serial.print(", which is a microsecond value of:");
    Serial.println(us_value,DEC);
  }
}

/*
  sendData [ISR]
  @brief Gets called whenever data has been requested from the organ over I2C
  Will send the contents of the send buffer. Other code must ensure this is the right data.
*/
void sendData() {
  Wire.write(send_buffer);
}

/*
  receiveData [ISR]
  @brief Gets called whenever bytes have arrived over I2C.
  Expect this to be a single data byte, or two bytes (register address plus data)
  @param received_data_byte_count Number of bytes received
*/
void receiveData(int received_data_byte_count){
  //Buffer the incoming byte(s)
  input_buffer[0] = Wire.read(); // register
  if (received_data_byte_count > 1) {
    input_buffer[1] = Wire.read(); // value
  } else {
    input_buffer[1] = 0;
  }

  //Handle different commands
  switch (input_buffer[0]) {
    
    //Servo enable or disable
    case SERVO_ENABLE_REGISTER :
      if (input_buffer[1] == false) {
        servoEnable(false);
      } else {  //Any nonzero value considered 'true'
        servoEnable(true);
      }
      break;

    //Set current limit
    case CURRENT_LIMIT_REGISTER :
      current_limit_setting = input_buffer[1];
      update_current_limit_flag = true;
      break;

    //Set target position
    case TARGET_POSITION_REGISTER :
      target_position_setting = input_buffer[1];
      update_target_position_flag = true;
       //Enable power to servo if not already
      if(!servo_is_started) {
        servoEnable(true);
      }
      break;

    //Set indicator LED brightness
    case LED_BRIGHTNESS_REGISTER :
      led_brightness = input_buffer[1];
      update_led_brightness_flag = true;
      break;

    //Read measured current
    case MEASURED_CURRENT_REGISTER :
      send_buffer = measured_current; //Update send buffer with most recent reading
      if (received_data_byte_count > 1){
        // should not be here! Reset the i2c bus
        Serial.println("reseting i2c");
        TWCR = 0;
        Wire.begin();
      }
      break;

    //Read measured position
    case MEASURED_POSITION_REGISTER :
      send_buffer = measured_position; //Update send buffer with most recent reading
      if (received_data_byte_count > 1){
        // should not be here! Reset the i2c bus
        Serial.println("reseting i2c");
        TWCR = 0;
        Wire.begin();
      }
      break;

    case SET_TEST_VALUE_REGISTER:
      test_register_value = input_buffer[1];
      break;
    
    case GET_TEST_VALUE_REGISTER:
      send_buffer=test_register_value;
      if (received_data_byte_count > 1){
        // should not be here! Reset the i2c bus
        Serial.println("reseting i2c");
        TWCR = 0;
        Wire.begin();
      }
      break;
    default:
      Serial.print("Attempt to access unknown register: 0x");
      Serial.println(input_buffer[0], HEX);
      break;
  }
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
