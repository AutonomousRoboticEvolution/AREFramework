#pragma once

#include "Environment.h"


class MoveEnv :
	public Environment
{
public:
	MoveEnv();
	~MoveEnv();
	void init() override;

    std::vector <float> pStart;
	/// starting point
    std::vector <float> pOne;
	/// end point
    std::vector <float> pEnd;

	float fitnessFunction(MorphologyPointer morph) override;
	float updateEnv(MorphologyPointer morph) override;

	///time point to check the status of the robot
	float timeCheck = 0.0;

};

