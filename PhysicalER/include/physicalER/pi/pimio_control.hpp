#ifndef PIMIO_CONTROL_HPP
#define PIMIO_CONTROL_HPP

#include <iostream>
#include <vector>
#include "physicalER/pi/dbusinterface.h"
#include "physicalER/pi/pi_individual.hpp"
#include <QTest>

namespace are {
namespace pi {

class PimioControl{
public:

    PimioControl(const NN2Individual& ind);

    void sendMotorCommand(double left, double right);
    void retrieveSensorValues(std::vector<double> &sensor_vals);

    int exec(int argc, char** argv, zmq::socket_t& socket);

private:
    Aseba::DBusInterface* interface;
    NN2Individual controller;
    float _max_eval_time = 30000;
    float _time_step = 100;
};

}//pi
}//are

#endif //PIMIO_CONTROL_HPP
