#include "MoveAndSun.h"



MoveAndSun::MoveAndSun()
{
	cout << "CREATED MOVE AND SUN" << endl;
}


MoveAndSun::~MoveAndSun()
{
}

void MoveAndSun::init() {
	envObjectHandles.clear(); // every time the environment is created, it removes all objects and creates them again
	Environment::init();
	maxTime = 30;
	checkMove = false;
	//float size[3] = { 1.0, 1.8, 0.02 };
	//int plate1 = simCreatePureShape(0, 24, size, 0, NULL);
	//int plate3 = simCreatePureShape(0, 24, size, 0, NULL);
	//size[0] = 1.8; size[1] = 1.0;
	//int plate2 = simCreatePureShape(0, 24, size, 0, NULL);
	//int plate4 = simCreatePureShape(0, 24, size, 0, NULL);
	//float pos[3] = { 0.7, 0.7, 0.01 };
	//simSetObjectPosition(plate1, -1, pos);
	//pos[0] = -0.7;
	//simSetObjectPosition(plate2, -1, pos);
	//pos[1] = -0.7;
	//simSetObjectPosition(plate3, -1, pos);
	//pos[0] = 0.7;
	//simSetObjectPosition(plate4, -1, pos);
	//envObjectHandles.push_back(plate1);
	//envObjectHandles.push_back(plate2);
	//envObjectHandles.push_back(plate3);
	//envObjectHandles.push_back(plate4);


	//	//Environment::init(); 
	//// create four walls: 
	//float wallSize[3] = {2,0.1,0.5};
	//float wallPos[4][3] = { 
	//	{ wallSize[0] / 2, 0, wallSize[2] / 2 },
	//	{ 0, wallSize[0] / 2, wallSize[2] / 2 },
	//	{ - wallSize[0] / 2, 0, wallSize[2] / 2 },
	//	{ 0, - wallSize[0] / 2, wallSize[2] / 2 }
	//};
	//float wallOr[4][3] = { 
	//	{ 0,0,0.5 * M_PI },
	//	{ 0,0,0 },
	//	{ 0,0, 0.5 * M_PI },
	//	{ 0,0, M_PI }
	//};
	//for (int i = 0; i < 4; i++) {
	//	envObjectHandles.push_back(simCreatePureShape(0, 24, wallSize, 1, NULL)); // they are static
	//	simSetObjectOrientation(envObjectHandles[i], -1, wallOr[i]);
	//	simSetObjectPosition(envObjectHandles[i], -1, wallPos[i]);
	//}
}

float MoveAndSun::fitnessFunction(MorphologyPointer morph) {
	float fitness = 0;
	if (type == 0) {
		if (morph->modular = false) {
			fitness = 0; // no fixed morphology that can absorb light
		}
		else {
			vector<shared_ptr<ER_Module> > createdModules = morph->getCreatedModules();
			fitness += BasicSunEnv::fitnessFunction(morph);
			vector<float> pos = createdModules[0]->getPosition();
			fitness += sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
		}
	}
	else if (type == 1) {
		float endEnergy = BasicSunEnv::fitnessFunction(morph);
		fitness = moveFitness + (endEnergy - startEnergy);
		cout << "moveFit = " << moveFitness << ", endEnergy = " << endEnergy << ", startEnergy = " << startEnergy << " and fitness = " << fitness << endl;
	}
	moveFitness = 0; 
	startEnergy = 0;
	return fitness;

}

float MoveAndSun::updateEnv(MorphologyPointer morph) {
	float fitness = 0;
	if (type == 0) {
		if (morph->modular = false) {
			fitness = 0; // no fixed morphology that can absorb light
		}
		else {
			BasicSunEnv::updateEnv(morph);
		}
	}
	else if (type == 1) {
		if (simGetSimulationTime() < maxTime * 0.5) {
			// add movement
			startEnergy = BasicSunEnv::updateEnv(morph);
		}
		else {
			BasicSunEnv::updateEnv(morph);
			if (checkMove == false) {
				int mainHandle = morph->getMainHandle();
				float pos[3];
				simGetObjectPosition(mainHandle, -1, pos);
				fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
				vector<float> pEnd;
				pEnd.push_back(pos[0]);
				pEnd.push_back(pos[1]);
				fitness = sqrtf((pEnd[0] * pEnd[0]) + (pEnd[1] * pEnd[1]));
				moveFitness = fitness;
				checkMove = true;
			}
			// addEnergy
		}
	}
	return fitness;
}