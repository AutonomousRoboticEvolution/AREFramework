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
  
    case REQUEST_INFRARED_REGISTER: // set control register
      break;

    case FLASH_INDICATOR_LED_REGISTER: // increase the requested number of flashes
      number_of_flashes_requested += input_buffer[1]; // this will be decrememented each time a flash is done, until it reaches zero
      break;

    case SET_TIME_OF_FLIGHT_ADDRESS_REGISTER:
      new_address_for_time_of_flight_sensor_received = true;
      new_address_of_time_of_flight_sensor = input_buffer[1];
      break;

    case GET_TIME_OF_FLIGHT_ADDRESS_REGISTER:
      break;

    case REQUEST_INFRARED_RAW_VALUE_REGISTER:
      break;

    case SET_TEST_VALUE_REGISTER:
      test_register_value = input_buffer[1];
        #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("new test_register_value: 0x");
        Serial.print(test_register_value, HEX);
        Serial.print("\t= ");
        Serial.println(test_register_value);
      #endif
      break;
    
    case SET_INFRARED_THREASHOLD_REGISTER:
      IR_threashold = input_buffer[1];

    
    case GET_TEST_VALUE_REGISTER:
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
   
  if(input_buffer[0] == REQUEST_INFRARED_REGISTER){
      Wire.write(reading);
      
      #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("Sending IR value: ");
        Serial.print(reading);
        Serial.print(" = 0x");
        Serial.println(reading,HEX);
      #endif
      

  }else if (input_buffer[0] == GET_TIME_OF_FLIGHT_ADDRESS_REGISTER) {
    Wire.write(current_address_of_time_of_flight_sensor);
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print("TOF address requested, sent: 0x");
      Serial.println(current_address_of_time_of_flight_sensor,HEX);
    #endif

  } else if (input_buffer[0] == REQUEST_INFRARED_RAW_VALUE_REGISTER) {
    // the pi will make two read8() requests, the first time we should send the MSB byte, and the second time the LSB byte
    if (!first_byte_has_been_sent){
      // first byte has not yet been sent, so compute the reading and send the first byte
      raw_reading =  analogRead(IR_MEASURE_PIN);
      
      //send the first (MSB) byte
      send_buffer = (raw_reading & 0xFF00 )>>8;
      first_byte_has_been_sent=true;
  
      #ifdef SERIAL_DEBUG_PRINTING
        Serial.print("Raw IR value: ");
        Serial.print(raw_reading);
        Serial.print(" = 0x");
        Serial.println(raw_reading,HEX);
      #endif
    } else { // we should send the second byte
      send_buffer = raw_reading & 0xFF;
      first_byte_has_been_sent=false;
    }
    // send the value:
    Wire.write(send_buffer);
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print("Sent: 0x");
      Serial.print(send_buffer,HEX);
      Serial.print("\t= ");
      Serial.println(send_buffer,BIN);
    #endif

  } else if (input_buffer[0] == GET_TEST_VALUE_REGISTER){
    Wire.write(test_register_value);
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.print("test register value us requested, sent: 0x");
      Serial.print(test_register_value,HEX);
      Serial.print("\t= ");
      Serial.println(test_register_value);
    #endif

  }else{
    #ifdef SERIAL_DEBUG_PRINTING
      Serial.println("ERROR! in sendData, but input_buffer[0] is not recognised as a readable register");
    #endif
  }
}
