#ifndef ARE_CONTROL_HPP
#define ARE_CONTROL_HPP

#define NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER 30.f

#include <iostream>
#include <vector>
#include "physicalER/pi/pi_individual.hpp"
#include "MotorOrgan.hpp"
#include "DaughterBoards.hpp"
#include "LedDriver.hpp"
#include <zmq.hpp>

namespace are {
namespace pi {

class AREControl{
public:

    AREControl(const NN2Individual& ind);

    void sendMotorCommands(std::vector<double> values);
    void retrieveSensorValues(std::vector<double> &sensor_vals);

    int exec(int argc, char** argv, zmq::socket_t& socket);

private:
    NN2Individual controller;
    float _max_eval_time = 30000000; // microseconds
    float _time_step = 100000; // microseconds

    std::shared_ptr<MotorOrgan> wheel0;
    std::shared_ptr<MotorOrgan> wheel1;
    std::shared_ptr<MotorOrgan> wheel2;
    std::shared_ptr<MotorOrgan> wheel3;
    std::shared_ptr<DaughterBoards> daughterBoards;
    std::shared_ptr<LedDriver> ledDriver;
};

}//pi
}//are

#endif //PIMIO_CONTROL_HPP
