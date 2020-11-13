#ifndef ARE_CONTROL_HPP
#define ARE_CONTROL_HPP

#include <iostream>
#include <vector>
#include "physicalER/pi/pi_individual.hpp"
#include "MotorOrgan.hpp"
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
    float _max_eval_time = 10; // seconds
    float _time_step = 0.1; // seconds
    
    std::shared_ptr<MotorOrgan> wheel0;
};

}//pi
}//are

#endif //PIMIO_CONTROL_HPP
