// interrupt service routines - these are called when the encoder ticks:
void A_rises(){
    encoder_A_is_high=true;
    if (encoder_B_is_high){increment_positive();}
    else {increment_negative();}
    attachInterrupt(PIN_ENCODER_A, A_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void A_falls(){
    encoder_A_is_high=false;
    if (encoder_B_is_high){increment_negative();}
    else {increment_positive();}
    attachInterrupt(PIN_ENCODER_A, A_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_rises(){
    encoder_B_is_high=true;
    if (encoder_A_is_high){increment_negative();}
    else {increment_positive();}
    attachInterrupt(PIN_ENCODER_B, B_falls, FALLING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}
void B_falls(){
    encoder_B_is_high=false;
    if (encoder_A_is_high){encoder_ticks_this_timestep ++;}
    else {encoder_ticks_this_timestep --;}
    attachInterrupt(PIN_ENCODER_B, B_rises, RISING); // start waiting for the other ISR - only one can be active on any particular pin at a time
}

// helper functions for incrementing/decrementing the encoder tracking:
void increment_positive(){
    last_tick_direction=1;
    encoder_ticks_this_timestep ++;
    time_of_last_but_one_tick = time_of_last_tick;
    time_of_last_tick=micros();
}
void increment_negative(){
    last_tick_direction=-1;
    encoder_ticks_this_timestep --;
    time_of_last_but_one_tick = time_of_last_tick;
    time_of_last_tick=micros();
}

// should be called once per timestep to update the measured velocity:
int get_measured_velocity_from_encoder(){
    int temp_value = encoder_ticks_this_timestep;
    encoder_ticks_this_timestep=encoder_ticks_this_timestep-temp_value;
    return temp_value;
}
