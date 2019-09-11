#pragma once
#include "Environment.h"
class MoveEnv :
	public Environment
{
public:
	MoveEnv();
	~MoveEnv();
	void init(); 

	vector <float> pStart;
	/// starting point
	vector <float> pOne;
	/// end point
	vector <float> pEnd;

	float fitnessFunction(MorphologyPointer morph);
	float updateEnv(MorphologyPointer morph);

	///time point to check the status of the robot
	float timeCheck = 0.0;

};

