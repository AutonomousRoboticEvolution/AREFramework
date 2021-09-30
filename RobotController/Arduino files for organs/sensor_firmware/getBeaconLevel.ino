void getBeaconLevel(){

  unsigned long start_us=micros();
  // Read raw IR measurement and save at SAMPLE_PERIOS_US rate
  for (uint16_t i=0; i<BUFFER_SIZE; i++){
    while(micros()<start_us+(i*SAMPLE_PERIOS_US)){}
    raw_values[i]= (analogRead(IR_MEASURE_PIN) );
  }
  
  uint16_t samples[WINDOW_SIZE];
  uint8_t reading = -1;
  
  for (int i=0 ; i<BUFFER_SIZE; i++){
    samples[i%WINDOW_SIZE] = raw_values[i];

    uint16_t min_value=-1;
    uint16_t max_value=0;
    for (int i_sample=0; i_sample<WINDOW_SIZE; i_sample++){
      if (samples[i_sample]<min_value) min_value=samples[i_sample];
      if (samples[i_sample]>max_value) max_value=samples[i_sample];
    }
    if (max_value-min_value < reading) reading = max_value-min_value;
  }

  if(set_LED_to_IR_value){
    if( IR_threashold >0 ){ // if IR_threashold is set to zero (the default starting value), we display the reading as PWM, otherwise we display a binary value, on is above threashold
      if (reading > IR_threashold) {digitalWrite(INDICATOR_LED_PIN,HIGH);} else {digitalWrite(INDICATOR_LED_PIN,LOW);}
    }else{
      analogWrite(INDICATOR_LED_PIN, map(constrain(reading,0,254), 0,254, 0,80));
    }
  }

  #ifdef SERIAL_DEBUG_IR_SENSOR
    Serial.print("Filtered beacon reading: ");
    Serial.println(reading);
  #endif
}
