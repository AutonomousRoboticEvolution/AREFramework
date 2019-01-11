#pragma once
#include "BasicSunEnv.h"

class MoveAndSun :
	public BasicSunEnv
{
public:
	MoveAndSun();
	~MoveAndSun();
	void init(); 
	float fitnessFunction(MorphologyPointer morph);
	float updateEnv(MorphologyPointer morph);
	float startEnergy = 0;
	float moveFitness = 0;
	bool checkMove = false; 

	float checkSunInterSection(vector<float> fourPoints, float objectMatrix[12], vector<shared_ptr<ER_Module> > createdModules);
};

