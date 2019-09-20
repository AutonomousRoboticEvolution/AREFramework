#pragma once

#include <vector>
#include "MoveEnv.h"


class EnvPhototaxis :
	public MoveEnv
{
public:
	EnvPhototaxis();
	~EnvPhototaxis();
	void init() override;
	float fitnessFunction(MorphologyPointer morph) override;
	float updateEnv(MorphologyPointer morph) override;
	float start(MorphologyPointer morph);

	float startEnergy = 0;
	float moveFitness = 0;
	bool checkMove = false; 
	int lightHandle = -1;
	float alphaRange = 1.5708; // 0.5 pi
	float lightDistance = 2.0;
	float lightHeight = 0.5;
	int interval = 4;
    std::vector<float> fitStorage;
};

