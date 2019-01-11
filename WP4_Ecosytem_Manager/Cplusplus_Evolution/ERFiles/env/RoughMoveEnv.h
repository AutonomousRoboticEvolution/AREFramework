#pragma once
#include "MoveEnv.h"
class RoughMoveEnv :
	public MoveEnv
{
public:
	RoughMoveEnv();
	~RoughMoveEnv();
	void init(); 
	float fitnessFunction(MorphologyPointer morph);
	float updateEnv(MorphologyPointer morph);

};

