#pragma once
#include "BasicSunEnv.h"

class MovingSunEnv :
	public BasicSunEnv
{
public:
	MovingSunEnv();
	~MovingSunEnv();
	void init(); 
	int lightHandle; 
	int iteration;
	int step; 	
	float fitnessFunction(MorphologyPointer morph);
	float updateEnv(MorphologyPointer morph);

	float checkSunInterSection(vector<float> fourPoints, float objectMatrix[12], vector<shared_ptr<ER_Module> > createdModules);
};

