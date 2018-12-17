#include "RoughMoveEnv.h"



RoughMoveEnv::RoughMoveEnv()
{
}


RoughMoveEnv::~RoughMoveEnv()
{
}

void RoughMoveEnv::init() {
	simLoadScene("scenes/RoughTerrainLight.ttt");
//	cout << "LOADING ROUGH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	initialPos.resize(3);
	initialPos[0] = 0.0;
	initialPos[1] = 0.0;
	initialPos[2] = 0.5;
	minimumHeigth = 0.3;
	timeCheck = 2.5;
	maxTime = 20.0;
//	cout << "environment Initialized" << endl;
//	simSetInt32Parameter(sim_intparam_dynamic_step_divider, 25);
	//envObjectHandles.clear();
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
}

float RoughMoveEnv::fitnessFunction(MorphologyPointer morph) {
	float fitness = 0;
	//if (morph->modular == false) {
	//	cout << "getting main handle" << endl;
	//	int mainHandle = morph->getMainHandle();
	//	float pos[3];
	//	simGetObjectPosition(mainHandle, -1, pos);
	//	fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
	//	//	fitness = 0; // no fixed morphology that can absorb light
	//}
	//else {
	//	int mainHandle = morph->getMainHandle();
	//	float pos[3];
	//	simGetObjectPosition(mainHandle, -1, pos);
	//	fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]) + sqrtf(pos[2] * pos[2]);

	//	//	vector<shared_ptr<ER_Module> > createdModules = morph->getCreatedModules();
	//	//	vector<float> pos = createdModules[0]->getPosition();
	//	//	fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
	//}
    fitness = MoveEnv::fitnessFunction(morph);

	return fitness;
}

float RoughMoveEnv::updateEnv(MorphologyPointer morph) {
	MoveEnv::updateEnv(morph);
	return 0;
}