//
// Created by matt on 18/05/2020.
//

#include "INA219_current_sensor.hpp"
#include <iostream>
#include <bitset>

//Constructor
INA219_current_sensor::INA219_current_sensor(uint8_t address) : I2CDevice(address){
    //Just uses the I2CDevice constructor
}

void display_binary(uint16_t value){
    std::cout << std::bitset<4>(value>>12) << " ";
    std::cout << std::bitset<4>((value>>8)) << " ";
    std::cout << std::bitset<4>((value>>4)) << " ";
    std::cout << std::bitset<4>((value)) << std::endl;
}

void INA219_current_sensor::init(){
    setCalibration_32V_2A();
}

uint16_t INA219_current_sensor::reverse_bits(uint16_t b) {
    b = (b & 0xFF00) >> 8 | (b & 0x00FF) << 8;
    b = (b & 0xF0F0) >> 4 | (b & 0x0F0F) << 4;
    b = (b & 0xCCCC) >> 2 | (b & 0x3333) << 2;
    b = (b & 0xAAAA) >> 1 | (b & 0x5555) << 1;
    return b;
}

float INA219_current_sensor::read_current(){
    uint16_t raw_value = read16From(REGISTER_CURRENT);
    raw_value = (raw_value & 0xFF00) >> 8 | (raw_value & 0x00FF) << 8; // switch over the two bits

//    std::cout<<"raw: ";
//    display_binary(raw_value);

    bool is_negative = (raw_value >> 15) & 0x01; // 1st bit is the sign value
    int16_t signed_value;
    if (is_negative) {//is negative
        raw_value = raw_value ^ 0xFFFF; // XOR to do 2's compliment
        signed_value = raw_value * -1;
    }else{ // is positive
        signed_value=raw_value;
    }

    return float(signed_value/10.0); // 100uA per bit, output in mA
}


float INA219_current_sensor::read_voltage(){
    uint16_t raw_value = read16From(REGISTER_BUSVOLTAGE);
    raw_value = (raw_value & 0xFF00) >> 8 | (raw_value & 0x00FF) << 8; // switch over the two bytes

//    std::cout<<"raw:\t";display_binary(raw_value);
    if ((raw_value & 0x0001) == 1){ // last bit is the math overflow warning
        printf("INA warning: math overflow!\n");
    }
    if ((raw_value & 0x0002)>>1 == 0){ // penultimate bit is the data ready bit
        printf("INA warning: data wasn't ready!\n");
//        std::cout<<"INA warning: data wasn't ready!\n";
    }

//    std::cout << "raw value:" << std::bitset<16>(raw_value)  << std::endl;
//    int16_t voltage_value = reverse_bits((raw_value&0xFFF8)>>3)>>3; // thirteen first bits, but in reverse order
    bool is_negative = (raw_value >> 15) & 0x01; // 1st bit is the sign value
    uint16_t voltage_value = (raw_value&0x7FE0)>>3; // the first 13 bits correspond to the actual value

    int16_t signed_voltage_value;
    if (is_negative) {//is negative
        printf("VOLTAGE IS NEGATIVE?!?!");
//        std::cout << "voltage value:" << std::bitset<16>(voltage_value)  << std::endl;
//        voltage_value = voltage_value & 0xFFF; // remove the sign bit
        voltage_value = voltage_value ^ 0x1FFF; // XOR to do 2's compliment
//        std::cout << "XOR'd:" << std::bitset<16>(voltage_value)  << std::endl;
        signed_voltage_value = voltage_value * -1;
//        std::cout << "signed value:" << std::bitset<16>(signed_voltage_value)  << std::endl;
    }else{ // is positive
        signed_voltage_value=voltage_value;
    }


//    std::cout << std::bitset<16>(signed_voltage_value)  << std::endl;
    return float(signed_voltage_value)/8000.0*32 ;
    // magic numbers here! max value (32V) is represented by a value of 8000 (0x1F40), see section 8.6.3.2 http://www.ti.com/lit/ds/symlink/ina219.pdf?&ts=1589900192086
}

void INA219_current_sensor::setCalibration_32V_2A() {
    uint16_t cal_value = 4096; // copied from https://github.com/adafruit/Adafruit_INA219/blob/master/Adafruit_INA219.cpp
    cal_value = (cal_value & 0xFF00) >> 8 | (cal_value & 0x00FF) << 8; // switch over the two bytes, because INA219 expects them in the other order from C++
    write16To(REGISTER_CALIBRATION,cal_value);
}