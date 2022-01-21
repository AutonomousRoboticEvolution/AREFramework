/*
  Authors: Mike Angus, Matt Hale
  Date: Sept 2021
  Description: Firmware for Joint Organ v1.2
*/

//I2C ADDRESS
#define SLAVE_ADDRESS 0x0A // <=== THIS NEEDS TO BE SET FOR EACH UNIQUE ORGAN

//CALIBRATION VALUES (replace with copy-paste from joint_calibration output)
#define CALIB_CENTRE_POSITION_US 1450
#define CALIB_ENCODER_VALUE_AT_MIN_ANGLE 110
#define CALIB_ENCODER_VALUE_AT_MAX_ANGLE 872

//DEBUG FLAG
#define DEBUG 0

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
//NOTE TO SELF: can use map() to map servo angles to encoder values

//CURRENT LIMITER DEFAULTS
#define DIG_POT_ADDRESS 0x2E //7-bit address of device. Wire library uses 7 bit addressing throughout
#define MIN_CURRENT_MA 330 //milliamps
#define MAX_CURRENT_MA 1250 //milliamps
#define DEFAULT_CURRENT_LIMIT MIN_CURRENT_MA

#define MAX_SETI_ANALOG_READING 950 //Determined by ADC reference voltage AREF, set by onboard resistor

//Servo position
#define SERVO_POS_READING_MIN 30
#define SERVO_POS_READING_MAX 1000
#define SERVO_POS_READING_RANGE (SERVO_POS_READING_MAX - SERVO_POS_READING_MIN)

//Libraries
#include <Wire.h>
#include <Servo.h>

//I/O pins
#define INDICATOR_LED 9 //No PWM as conflicts with Servo library. Change to pin 6 for v1.2+ boards
#define I2C_ENABLE 15
#define SERVO_ENABLE 6 //change this to pin 7 for v1.2+ boards
#define SERVO_PWM 5
#define SETI_PIN 6 //Current measurement
#define SERVO_POSITION_PIN A3

//Register addresses
#define SERVO_ENABLE_REGISTER 0X10
#define TARGET_POSITION_REGISTER 0x11
#define MEASURED_POSITION_REGISTER 0x12
#define MEASURED_CURRENT_REGISTER 0x13
#define CURRENT_LIMIT_REGISTER 0x14
#define LED_BRIGHTNESS_REGISTER 0x15

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
  Serial.print("My address is 0x");
  Serial.println(SLAVE_ADDRESS,HEX);

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
}

/******MAIN LOOP********************/
void loop() {

  //Update measurements
  measured_position = readServoPosition();
  measured_current = readServoCurrent();

  //Update settings
  if (update_current_limit_flag) {
    setCurrentLimit(current_limit_setting);
    update_current_limit_flag = false;
  }
  
  if (update_target_position_flag) {
    setTargetPosition(target_position_setting);
    update_target_position_flag = false;
  }
  
  if (update_led_brightness_flag){
    update_led_brightness_flag=false;
    setLED(led_brightness);
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
  //Enable power to servo if not already
  if(!servo_is_started) {
    servoEnable(true);
  }
  
  //Calculate microseconds value and move servo
  //Note that for any calibrated centrepoint which is not exactly halfway in the PWM range,
  //the servo will not actually be able to reach the full extent in one of the directions
  unsigned int us_value = CALIB_CENTRE_POSITION_US + (float)target_position/DEG_PER_US;
  joint_servo.writeMicroseconds(us_value);

  //Debug info
  if (DEBUG) {
    Serial.print("I've been told to set the position to: ");
    Serial.print(target_position,DEC);
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
      break;

    //Set indicator LED brightness
    case LED_BRIGHTNESS_REGISTER :
      led_brightness = input_buffer[1];
      update_led_brightness_flag = true;
      break;

    //Read measured current
    case MEASURED_CURRENT_REGISTER :
      send_buffer = measured_current; //Update send buffer with most recent reading
      break;

    //Read measured position
    case MEASURED_POSITION_REGISTER :
      send_buffer = measured_position; //Update send buffer with most recent reading
      break;

    default:
      Serial.print("Attempt to access unknown register: 0x");
      Serial.println(input_buffer[0], HEX);
      break;
  }
}
