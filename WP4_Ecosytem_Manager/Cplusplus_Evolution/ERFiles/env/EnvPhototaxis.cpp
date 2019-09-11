#include "EnvPhototaxis.h"



EnvPhototaxis::EnvPhototaxis()
{
	cout << "CREATED MOVE AND SUN" << endl;
}


EnvPhototaxis::~EnvPhototaxis()
{
}

void EnvPhototaxis::init() {
	envObjectHandles.clear(); // every time the environment is created, it removes all objects and creates them again
	Environment::init();
	maxTime = 30;
	checkMove = false;
	lightHandle = simGetObjectHandle("Light");
}


float EnvPhototaxis::fitnessFunction(MorphologyPointer morph) {
	float fitness = 0;
	int mh = morph->getMainHandle();
	float posM[3];
	float posL[3];
	simGetObjectPosition(mh, -1, posM);
	simGetObjectPosition(lightHandle, -1, posL);
	// fitness is distance from light (sensing should evolve)
	fitness = sqrt(pow(posM[0] - posL[0], 2) * pow(posM[1] - posL[1], 2));
	return fitness;
}

float EnvPhototaxis::start(MorphologyPointer morph) {
	lightHandle = simGetObjectHandle("Light");
	lightDistance;
	float angle = alphaRange * morph->randomNum->randFloat(-1.0, 1.0);
	float x = cos(angle)* lightDistance;
	float y = sin(angle)* lightDistance;
	float l_pos[3];
	l_pos[0] = x;
	l_pos[1] = y;
	l_pos[2] = lightHeight;
	simSetObjectPosition(lightHandle, -1, l_pos);
	return 0.0f;
}



float EnvPhototaxis::updateEnv(MorphologyPointer morph) {
	float fitness = 0.0;

	return fitness;
}