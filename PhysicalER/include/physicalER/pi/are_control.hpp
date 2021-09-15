#ifndef ARE_CONTROL_HPP
#define ARE_CONTROL_HPP

#define DEBUGGING_INPUT_OUTPUT_DISPLAY true
#define VERBOSE_DEBUG_PRINTING_AT_SETUP true

#include <iostream>
#include <vector>
#include "physicalER/pi_individual.hpp"
#include "MotorOrgan.hpp"
#include "SensorOrgan.hpp"
#include "DaughterBoards.hpp"
#include "LedDriver.hpp"
#include <zmq.hpp>
#include <wiringPi.h> //for timing functions


namespace are {
namespace pi {

class AREControl{
public:

    AREControl(const phy::NN2Individual& ind , std::string stringListOfOrgans , settings::ParametersMapPtr parameters);

    void sendMotorCommands(std::vector<double> values);
    void retrieveSensorValues(std::vector<double> &sensor_vals);

    int exec( zmq::socket_t& socket);

private:
    phy::NN2Individual controller;
    float _max_eval_time ; // millieconds
    float _time_step = 1000; // milliseconds

    std::list<Organ*> listOfOrgans;

    std::shared_ptr<DaughterBoards> daughterBoards;
    std::shared_ptr<LedDriver> ledDriver;
};

}//pi
}//are

#endif //PIMIO_CONTROL_HPP
