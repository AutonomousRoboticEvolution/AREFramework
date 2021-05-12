#ifndef TESTENV_H
#define TESTENV_H

#include <cmath>
#include "ARE/Environment.h"
#include "ARE/Individual.h"

namespace are {

class TestEnv : public Environment
{
public:
    TestEnv() : Environment()
    {
        initial_position.resize(3);
        final_position.resize(3);
        name = "testEnv";
    }
    ~TestEnv(){}
//    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    const std::vector<double> &get_final_position(){return final_position;}

    ///time point to check the status of the robot
    float timeCheck = 0.0;
private:
    std::vector<double> initial_position;
    std::vector<double> final_position;
};

} //are

#endif //TESTENV_H
