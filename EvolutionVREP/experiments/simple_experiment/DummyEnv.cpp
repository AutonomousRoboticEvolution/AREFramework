#include "DummyEnv.h"


using namespace are;

void DummyEnv::init()
{
	Environment::init();
	timeCheck = 2.5;
	maxTime = 60.0;
}

double DummyEnv::fitnessFunction(Individual::Ptr ind)
{
    double fitness = 0;
	
    int mainHandle = ind->get_morpholgy()->getMainHandle();
    float pos[3];
    simGetObjectPosition(mainHandle, -1, pos);
    fitness = pos[0] + pos[1] + pos[2];

	return fitness;
}

float DummyEnv::updateEnv(MorphologyPointer morph)
{

	return 0;
}

