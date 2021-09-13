#include "Organ.hpp"

Organ::Organ(uint8_t address) : I2CDevice(address){
    //Just uses the I2CDevice constructor
}


void Organ::set_test_value(uint8_t value){
    write8To(SET_TEST_VALUE_REGISTER, value);
}
uint8_t Organ::get_test_value(){
    return read8From(GET_TEST_VALUE_REGISTER);
}

//Check for successful i2c communication with this organ
bool Organ::testConnection(){
    int value_to_send = 1 + rand() % 250;
    set_test_value(value_to_send);
    int returned_value = get_test_value();
    //std::cout<<"sent: "<<value_to_send<<", got: "<<returned_value<<std::endl;
    if (returned_value == value_to_send){
        return true;
    }else{
        return false;
    }
}
