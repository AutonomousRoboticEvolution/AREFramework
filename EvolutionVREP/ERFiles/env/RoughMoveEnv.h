#pragma once
#include "MoveEnv.h"
class RoughMoveEnv :
	public MoveEnv
{
public:
	RoughMoveEnv();
	~RoughMoveEnv();
	void init() override;
	float fitnessFunction(MorphologyPointer morph) override;
	float updateEnv(MorphologyPointer morph) override;

	float minimumHeigth = 0.0;

};

