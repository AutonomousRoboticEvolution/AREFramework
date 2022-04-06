#ifndef ARE_CONTROL_HPP
#define ARE_CONTROL_HPP

#define VERBOSE_DEBUG_PRINTING_AT_SETUP true

#include <iostream>
#include <sstream>
#include <vector>
#include "physicalER/pi_individual.hpp"
#include "physicalER/pi_communication.hpp"
#include "MotorOrgan.hpp"
#include "JointOrgan.hpp"
#include "SensorOrgan.hpp"
#include "BatteryMonitor.hpp"
#include "DaughterBoards.hpp"
#include "LedDriver.hpp"
#include "Camera.hpp"
#include <zmq.hpp>
#include <wiringPi.h> //for timing functions


namespace are {
namespace pi {

class AREControl{
public:

    AREControl(const phy::NN2Individual& ind , std::string stringListOfOrgans , settings::ParametersMapPtr parameters);
    ~AREControl(){
        for(auto &o: listOfOrgans)
            delete o;
    }

    int exec( zmq::socket_t& socket);

private:
    phy::NN2Individual controller;
    u_int32_t _max_eval_time ; // millieconds
    float _time_step ; // milliseconds

    BatteryMonitor batteryMonitor; // the battery monitor object
    Camera camera; // the camera object

    std::vector<Organ*> listOfOrgans; // a list of (pointers to) all the organ objects

    std::shared_ptr<DaughterBoards> daughterBoards;
    std::shared_ptr<LedDriver> ledDriver;

    bool cameraInputToNN;
    bool debugDisplayOnPi=false; // Don't change here, set by defining #debugDisplayOnPi,bool,1 in the paramters file. Determines whether to show show input/output values in terminal.
    bool debugLEDsOnPi=false; // Don't change here, set by defining #debugLEDsOnPi,bool,1 in the paramters file. Determines whether to show input/output values as Head LEDs.

    std::ostringstream logs_to_send;
    int number_of_sensors=0;
    int number_of_wheels=0;
    int number_of_joints=0;

    // private functions:

    /**
        @brief sendOutputOrganCommands takes the controller output and sends them to the wheels and joints
        @param values a vector of the outputs as produced by (for example) the neural network.
        Each value should generally be between -1.0 and 1.0
    */
    void sendOutputOrganCommands(const std::vector<double> &values, uint32_t time_milli);

    /**
        @brief retrieveSensorValues get the values from each of the robot's sensors
        @param sensor_vals a vector of the inputs for the controller, to be populated.
        Each value will generally be between 0.0 and 1.0
    */
    void retrieveSensorValues(std::vector<double> &sensor_vals);

    /**
     * @brief setLedDebugging sets the LEDs in the Head to show something about what is happening, to help a human to understand and debug problems
     * The first LED brightness is set GREEN in proportion to the largest TOF sensor reading
     * The second LED brightness is set RED in proportion to the closest IR value (brighter means closer)
     * The third LED is set BLUE in proportion to the magnitude of the largest motor or joint output
     * @param nn_inputs
     * @param nn_outputs
     */
    void setLedDebugging(std::vector<double> &nn_inputs,std::vector<double> &nn_outputs);
};

}//pi
}//are

#endif //PIMIO_CONTROL_HPP
