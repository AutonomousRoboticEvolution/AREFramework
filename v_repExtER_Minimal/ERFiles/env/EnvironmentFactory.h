#pragma once
#include "Environment.h"
#include "MoveEnv.h"
#include "RoughMoveEnv.h"
#include "WaterEnv.h"
#include <memory>

class EnvironmentFactory
{
public:
	EnvironmentFactory();
	~EnvironmentFactory();
	shared_ptr<Environment> createNewEnvironment(shared_ptr<Settings> st);
};

