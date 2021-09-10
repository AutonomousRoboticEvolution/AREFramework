#ifndef ORGAN_HPP
#define ORGAN_HPP

#include "I2CDevice.hpp"
#include "DaughterBoards.hpp"
#include <stdlib.h>     /* rand */

#define SET_TEST_VALUE_REGISTER 0x90 // save a given value
#define GET_TEST_VALUE_REGISTER 0x91 // return the saved value

/**
 * @brief The Organ class
 * This is the super-class for any organ attached to the i2c bus (e.g. sensors, wheels, joints)
 * It inherits from the I2CDevice class, and so provides all those functions (e.g. write8() etc.) to the derived class.
 */

class Organ : protected I2CDevice
{
public:
    /**
            @brief Organ constructor.
            This simply calls the superclass constructor I2CDevice()
            @param address The I2C device address, using 7-bit version
             (see I2CDevice class description for explanation)
    */
    Organ(uint8_t address);

    /**
     * @brief testConnection
     * @return bool whether the organ can be successfully communicated with on the i2c bus
     */
    bool testConnection();

private:

    /**
     * @brief set_test_value
     * sends the given value to the organ to be saved into a test register
     * @param value: the value to which the test register will be set
     */
    void set_test_value(uint8_t value);

    /**
     * @brief get_test_value
     * Will try to commuincate with the organ to retrieve the test register value.
     * Should return whatever value the test register was previously set to using set_test_value()
     * @return value
     */
    uint8_t get_test_value();
};

#endif // ORGAN_HPP
