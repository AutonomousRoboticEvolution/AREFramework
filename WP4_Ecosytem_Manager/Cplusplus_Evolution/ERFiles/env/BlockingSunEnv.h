#pragma once
#include "BasicSunEnv.h"

class BlockingSunEnv :
	public BasicSunEnv
{
public:
	BlockingSunEnv();
	~BlockingSunEnv();
	void init(); 
	float fitnessFunction(MorphologyPointer morph);
	MorphologyPointer morph;
	float updateEnv(MorphologyPointer morph);
	float checkSunInterSection(vector<float> fourPoints, float objectMatrix[12], vector<shared_ptr<ER_Module> > createdModules);
};

