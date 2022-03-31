#ifndef COPPELIA_COMMUNICATION_HPP
#define COPPELIA_COMMUNICATION_HPP

#include <iostream>
#include <functional>
#include <cmath>
#include <vector>

#include <boost/algorithm/string.hpp>

#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

namespace are {

    typedef struct lookup_table {
// look up table for sensor reading from "20.40.025 (raw) data for organ calibrations"
// range from 0-1m in the z direction and 0-0.3m in the x direction
        static int coord2value_map[11][4];
//threshold for passive IR
        static int threshold;
    }lookup_table;
namespace sim {


/**
 * @brief read the values of the proximity sensors corresponding to the distance to the nearest object detected
 * @param Handles of the proximity sensors
 * @param (Output) sensor values
 */
void readProximitySensors(const std::vector<int> handles, std::vector<double> &sensorValues);

/**
 * @brief read the values of the proximity sensors corresponding to the distance to the nearest object detected
 * @param Handles of the passive IR sensors
 * @param (Output) sensor values
 */
void readPassivIRSensors(const std::vector<int> handles, std::vector<double> &sensorValues);

/**
 * @brief read the values of the proximity sensors corresponding to the distance to the nearest object detected
 * @param Handles of the passive IR sensors
 * @param (Output) sensor values
 */
void readCamera(const int camera_handle, std::vector<double> &sensorValues);


/**
 * @brief Retrieve the handles of all of the organs of the robot under the main handle
 * @param Handle of the robot
 * @param (Output) list of the handles of the poximity sensors (time of flight)
 * @param (Output) list of the handles of the passive IR sensors
 * @param (Output) list of the handles of the wheel
 * @param (Output) list of the handles of the joints
 */
void retrieveOrganHandles(int mainHandle, std::vector<int> &proxHandles, std::vector<int> &IRHandles,
                          std::vector<int> &wheelHandles, std::vector<int> &jointHandles, int &camera_handle);


/**
 * @brief sent command to the joint. The commands is passed directly as position to the joint.
 * @param handles of the joint
 * @param commands in (-1,1)
 */
void sentCommandToJointsDirect(const std::vector<int>& handles,const std::vector<double>& commands);

/**
 * @brief Proportional controller.
 * @param handles of the joints
 * @param commands in (-1,1)
 * @param Proportional Gain
 */
void sentCommandToJointsProportional(const std::vector<int>& handles, const std::vector<double>& commands, double P);

/**
 * @brief TODO : PID controller
 * @param handles
 * @param commands
 * @param Proportional gain
 * @param Integral gain
 * @param Derivative gain
 */
//void sentCommandToJointsPID(const std::vector<int>& handles, const std::vector<double>& commands, double P, double I, double D);

/**
 * @brief Oscillatory controller based on a sinusoïdal function
 * @param handles of the joints
 * @param commands in (-1,1)
 */
void sentCommandToJointsOscillatory(const std::vector<int> &handles, const std::vector<double> &commands);

/**
 * @brief get joints' positions
 * @param list of the handles of the joints
 * @param (output) list of positions of the joints
 */
void getJointsPosition(const std::vector<int>& handles, std::vector<double>& positions);

/**
 * @brief sent command to the wheels
 * @param handles of the wheel
 * @param commands in (-1,1)
 * @param absolute maximum velocity
 */
void sentCommandToWheels(const std::vector<int>& handles, const std::vector<double>& commands, double max_velocity);

/**
 * @brief print on the standard output the name of the state corresponding to the argument
 * @param simulator state integer code
 */
void printSimulatorState(int simState);

void robotScreenshot(int ind_id, std::string repository);

}//sim

}//are

#endif //COPPELIA_COMMUNICATION_HPP
