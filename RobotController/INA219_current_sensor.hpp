/**
 * @author Matt
 * @brief Temporary code for using my "benchtop" Head organ, this communicates with the INA219 current sensor
 */

#ifndef ROBOTCONTROLLER_INA219_CURRENT_SENSOR_H
#define ROBOTCONTROLLER_INA219_CURRENT_SENSOR_H

#include <stdint.h>
#include "I2CDevice.hpp"

// register values:
#define REGISTER_BUSVOLTAGE 0x02
#define REGISTER_CURRENT 0x04
#define REGISTER_CALIBRATION 0x05



class INA219_current_sensor : protected I2CDevice {
public:
    INA219_current_sensor(uint8_t address);

    void init();
    float read_current();
    float read_voltage();
    void setCalibration_32V_2A();

    uint16_t reverse_bits(uint16_t b);

};


#endif //ROBOTCONTROLLER_INA219_CURRENT_SENSOR_H
