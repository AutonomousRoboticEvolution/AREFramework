#ifndef TESTENV_H
#define TESTENV_H

#include "ARE/Environment.h"
#include "ARE/Individual.h"

namespace are {

class TestEnv : public Environment
{
public:
    TestEnv() : Environment(){}
    ~TestEnv(){}
//    void init() override;

    double fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;
};

} //are

#endif //TESTENV_H
