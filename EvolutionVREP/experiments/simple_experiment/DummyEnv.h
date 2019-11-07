#ifndef DUMMYENV_H
#define DUMMYENV_H

#include "ARE/Environment.h"
#include "ARE/Individual.h"

namespace are {

class DummyEnv : public Environment
{
public:
    DummyEnv() : Environment(){}
    ~DummyEnv(){}
    void init() override;

    double fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

};

} //are

#endif //DUMMYENV_H
