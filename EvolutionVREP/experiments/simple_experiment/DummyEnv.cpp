#include "DummyEnv.h"


using namespace are;

void DummyEnv::init()
{
	Environment::init();
}

double DummyEnv::fitnessFunction(const Individual::Ptr &ind)
{
    double fitness = 0;
	
    int mainHandle = ind->get_morphology()->getMainHandle();
    float pos[3];
    simGetObjectPosition(mainHandle, -1, pos);
    fitness = pos[0] + pos[1] + pos[2];

	return fitness;
}

float DummyEnv::updateEnv(float simulationTime, const Morphology::Ptr &morph)
{

	return 0;
}

