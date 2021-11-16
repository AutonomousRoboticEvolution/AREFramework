#ifndef TESTENV_H
#define TESTENV_H

#include <cmath>
#include "/home/xukang/Project/Autonomous_Robotics_Envolutionary/evolutionary_robotics_framework/simulation/include/simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"

#if defined(VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

namespace are {

class TestEnv : public sim::VirtualEnvironment
{
public:
    TestEnv() : VirtualEnvironment()
    {
        initial_position.resize(3);
        final_position.resize(3);
        name = "testEnv";
    }
    ~TestEnv(){}
//    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const sim::Morphology::Ptr &morph) override;

    const std::vector<double> &get_final_position(){return final_position;}

    ///time point to check the status of the robot
    float timeCheck = 0.0;
private:
    std::vector<double> initial_position;
    std::vector<double> final_position;
};

} //are

#endif //TESTENV_H
