// controller parameters
#define K_P 10.0
#define K_I 0.5
#define MAX_INTEGRAL_VALUE 150 // for anti-windup, the integral term will never have magnitude greater than this

//Slew rate limiting parameter (max allowed value change per timestep)
//Total output range is -255 to +255 (510 total), so e.g. a value of 10 here would take 51 timesteps to do a maximum end-end swing
//For a loop time of 10ms this example would be 510ms
//A rate of 125 of more is too fast (motor cuts out). 100 appears to be ok but is probably borderline.
#define MAX_OUTPUT_SLEW_RATE 50 //Motor power demand output cannot change by more than this per timestep.

int sign(int number){
    if (number==0){return 0;}
    else if (number<0){return -1;}
    else{ return 1;}
}

// will return the raw motor power value, in the range -255 to +255
// both demand_velocity and measured_velocity are measured in ticks-per-timestep.
int controller( int demand_velocity, int measured_velocity ){
    // open-loop controller
    float controller_value;    
    controller_value = FF_controller(demand_velocity);

    // compute new integral_value
    integral_value += K_I* (demand_velocity-measured_velocity);
    integral_value = constrain(integral_value,-MAX_INTEGRAL_VALUE,MAX_INTEGRAL_VALUE);

    // add on proportional and integral components:
    controller_value += K_P*(demand_velocity-measured_velocity) + integral_value;

    if (abs(controller_value)>255){return sign(controller_value)*255;}
    else {return int(controller_value);}
}

// will return the raw motor power value, in the range -255 to +255, using only the a lookup table to make a feedforward controller
float FF_controller(int demand_velocity){
int table[][2] = {{0, 0}, {40,8}, {80,23}, {120,29}, {240,35}};
int x0, x1, y0, y1;
for (int i = 0; i < 5; i++){ // loop through each of the "brackets" of the table
  if ( abs(demand_velocity) <= table[i + 1][1]){ // we are in the right bracket
    y0 = table[i][1];  //lower bound
      y1 = table[i + 1][1]; //upper bound
      x0 = table[i][0];
      x1 = table[i + 1][0];
      return sign(demand_velocity) * float(table[i][0] + ((table[i + 1][0] - table[i][0] ) * (float(abs(demand_velocity) - table[i][1]) / float(table[i + 1][1] - table[i][1])))); // linear interpolation
      //                                  (x0          + ((x1              - x0          ) * (     (y                    - y0         ) /      (y1              - y0         ))));
    }
  }
  // if we get here, demand must be greater than highest table value, so return max power
  return sign(demand_velocity) * 240;
}

void set_motor_power_value(int target_power){

    //Check the magnitude of the demanded change in output
    int change_magnitude = absDiff(target_power, last_output_power);
    int output_power_set;

    //If target value is too far from previous value, constrain the magnitude of the change to restrict the slew rate
    if (change_magnitude > MAX_OUTPUT_SLEW_RATE) {
        //Add (or subtract, for negative the maximum increment from the last output power if demanded slew rate is too fast
        output_power_set = (target_power > last_output_power) ? (last_output_power + MAX_OUTPUT_SLEW_RATE) : (last_output_power - MAX_OUTPUT_SLEW_RATE);
    } else {
        //Otherwise can go straight to the full target power
        output_power_set = target_power;
    }

    //Set the motor output to the target value
    if (output_power_set<0){
        // negative
        digitalWrite(PIN_MOTOR_DIR_PIN,LOW);
        analogWrite(PIN_MOTOR_PWM_PIN,-output_power_set);
    }
    else if (output_power_set>0){
        // positive
        digitalWrite(PIN_MOTOR_DIR_PIN,HIGH);
        analogWrite(PIN_MOTOR_PWM_PIN,output_power_set);
    } else { //zero
        digitalWrite(PIN_MOTOR_PWM_PIN,0);
    }

    //Update the global variable for the last output value
    last_output_power = output_power_set;

    //Display power on indicator LED
    #ifdef INDICATOR_LED_SHOW_MOTOR_POWER
        analogWrite(PIN_INDICATOR_LED, abs(output_power_set));
    #endif
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
