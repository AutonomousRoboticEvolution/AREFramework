#pragma once
#include "BasicSunEnv.h"

class ConstrainedSunEnv :
	public BasicSunEnv
{
public:
	ConstrainedSunEnv();
	~ConstrainedSunEnv();
	void init(); 
	float fitnessFunction(MorphologyPointer morph);
	MorphologyPointer morph;
	float updateEnv(MorphologyPointer morph);
	float checkSunInterSection(vector<float> fourPoints, float objectMatrix[12], vector<shared_ptr<ER_Module> > createdModules);
	int lightHandle;
	int iteration;
	int step;


};

