// triggered on every i2c conversation:
void receiveData(int received_data_byte_count){
  // the first byte received is the register value, and determines what we should do
  input_buffer[0] = Wire.read(); // register
  if (received_data_byte_count>1){
    input_buffer[1] = Wire.read(); // value
  #ifdef SERIAL_DEBUG_PRINTING
    if (received_data_byte_count>1){
      Serial.print("Received: 0x");
      Serial.print(input_buffer[0],HEX);
      Serial.print(", 0x");
      Serial.println(input_buffer[1],HEX);
    }else{
      Serial.print("Received: 0x");
      Serial.println(input_buffer[0],HEX);
    }
  #endif
  }else{
    input_buffer[1] = 0;
  }

  switch (input_buffer[0]) {
    case REQUEST_TIME_OF_FLIGHT_REGISTER: // set control register
      time_of_flight_value_is_requested=true;
      break;
  
    case REQUEST_INFRARED_REGISTER: // set control register
      infrared_value_is_requested=true;
      break;

    case FLASH_INDICATOR_LED_REGISTER: // increase the requested number of flashes
      number_of_flashes_requested += input_buffer[1]; // this will be decrememented each time a flash is done, until it reaches zero
      break;

    case SET_TIME_OF_FLIGHT_ADDRESS_REGISTER:
      new_address_for_time_of_flight_sensor_received = true;
      new_address_of_time_of_flight_sensor = input_buffer[1];
      break;

    case GET_TIME_OF_FLIGHT_ADDRESS_REGISTER:
      TOF_address_is_requested = true;
      break;

//    case SEARCH_FOR_TIME_OF_FLIGHT_ADDRESS:
//      findI2CAddress();
//      break;

    case REQUEST_INFRARED_RAW_VALUE_REGISTER:
      raw_IR_value_is_requested = true;
      break;
     
    default:
      #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("Attempt to access unknown register: 0x");
        Serial.println(input_buffer[0], HEX);
      #endif
      break;
  }
}

// gets called whenever master wants to read a register:
void sendData(){
   
  if(infrared_value_is_requested){
    // the pi will make two read8() requests, the first time we should send the MSB byte, and the second time the LSB byte
    if (!first_byte_has_been_sent){
      // first byte has not yet been sent, so compute the reading and send the first byte
      reading = getBeaconLevel(&irWindow, IR_WINDOW_SIZE, IR_WINDOW_WRAPPING_BITMASK);
      
      //send the first (MSB) byte
      send_buffer = (reading & 0xFF00 )>>8;
      first_byte_has_been_sent=true;
  
      #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("IR value: ");
        Serial.print(reading);
        Serial.print(" = 0x");
        Serial.println(reading,HEX);
      #endif
    } else { // we should send the second byte
      send_buffer = reading & 0xFF;
      first_byte_has_been_sent=false;
      infrared_value_is_requested=false;
    }
    // send the value:
    Wire.write(send_buffer);
    
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print("Sent: 0x");
      Serial.print(send_buffer,HEX);
      Serial.print("\t= ");
      Serial.println(send_buffer,BIN);
    #endif

  }else if (TOF_address_is_requested) {
    TOF_address_is_requested = false;
    Wire.write(current_address_of_time_of_flight_sensor);
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print("TOF_address_is_requested, sent: 0x");
      Serial.println(current_address_of_time_of_flight_sensor,HEX);
    #endif

  } else if (raw_IR_value_is_requested) {
    // the pi will make two read8() requests, the first time we should send the MSB byte, and the second time the LSB byte
    if (!first_byte_has_been_sent){
      // first byte has not yet been sent, so compute the reading and send the first byte
      reading =  analogRead(IR_MEASURE_PIN);
      
      //send the first (MSB) byte
      send_buffer = (reading & 0xFF00 )>>8;
      first_byte_has_been_sent=true;
  
      #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("Raw IR value: ");
        Serial.print(reading);
        Serial.print(" = 0x");
        Serial.println(reading,HEX);
      #endif
    } else { // we should send the second byte
      send_buffer = reading & 0xFF;
      first_byte_has_been_sent=false;
      raw_IR_value_is_requested=false;
    }
    // send the value:
    Wire.write(send_buffer);
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print("Sent: 0x");
      Serial.print(send_buffer,HEX);
      Serial.print("\t= ");
      Serial.println(send_buffer,BIN);
    #endif
    
  }else{
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.println("ERROR! in sendData, but no request flag is true");
    #endif
    reading=0;
  }
}
