#pragma once

#include "env/Environment.h"


class DummyEnv : public Environment
{
public:
    DummyEnv() : Environment(){}
    ~DummyEnv(){}
    void init() override;

    float fitnessFunction(MorphologyPointer morph) override;
    float updateEnv(MorphologyPointer morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

};
