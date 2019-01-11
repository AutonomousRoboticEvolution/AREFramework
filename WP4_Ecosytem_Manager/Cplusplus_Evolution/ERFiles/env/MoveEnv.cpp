#include "MoveEnv.h"



MoveEnv::MoveEnv()
{
}


MoveEnv::~MoveEnv()
{
}

void MoveEnv::init() {
	Environment::init();
	timeCheck = 2.5;
	maxTime = 20.0;
}

float MoveEnv::fitnessFunction(MorphologyPointer morph) {
	float fitness = 0;
	
//	int mainHandle = morph->getMainHandle();
//	float pos[3];
//	simGetObjectPosition(mainHandle, -1, pos);
//	return pos[0];
	if (settings->moveDirection == settings->FORWARD_Y) {
		if (morph->modular == false) {
			int mainHandle = morph->getMainHandle();
			float pos[3];
			simGetObjectPosition(mainHandle, -1, pos);
			fitness = -pos[1];
			pEnd.push_back(pos[1]);
			if (pOne.size() < 1) {
	//			cout << "Note, pOne never set" << endl;
			}
			else {
				fitness = fitness + pOne[1];
			}
			pOne.clear();
			pEnd.clear();
		}
		else {
			int mainHandle = morph->getMainHandle();
			float pos[3];
			simGetObjectPosition(mainHandle, -1, pos);
			fitness = -pos[1];
			pEnd.push_back(-pos[1]);
			if (pOne.size() < 1) {
				cout << "Note, pOne never set" << endl;
			}
			else {
				fitness = fitness + pOne[1];
			}

			// old
			//int brokenModules = morph->getAmountBrokenModules();
			//fitness = fitness * pow(0.8, brokenModules);
			pOne.clear();
			pEnd.clear();
		}
	}
	else {
		if (morph->modular == false) {
	//		cout << "getting main handle" << endl;
			int mainHandle = morph->getMainHandle();
			float pos[3];
			simGetObjectPosition(mainHandle, -1, pos);
			fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
			pEnd.push_back(pos[0]);
			pEnd.push_back(pos[1]);
			if (pOne.size() < 1) {
	//			cout << "Note, pOne never set" << endl;
				fitness = sqrtf((pEnd[0] * pEnd[0]) + (pEnd[1] * pEnd[1]));
			}
			else {
				fitness = sqrtf(((pEnd[0] - pOne[0]) * (pEnd[0] - pOne[0])) + ((pEnd[1] - pOne[1]) * (pEnd[1] - pOne[1])));
			}
			pOne.clear();
			pEnd.clear();
			//	fitness = 0; // no fixed morphology that can absorb light
		}
		else {
			int mainHandle = morph->getMainHandle();
			float pos[3];
			simGetObjectPosition(mainHandle, -1, pos);

			pEnd.push_back(pos[0]);
			pEnd.push_back(pos[1]);
			if (pOne.size() < 1) {
	//			cout << "Note, pOne never set" << endl;
				fitness = sqrtf((pEnd[0] * pEnd[0]) + (pEnd[1] * pEnd[1]));
			}
			else {
				fitness = sqrtf(((pEnd[0] - pOne[0]) * (pEnd[0] - pOne[0])) + ((pEnd[1] - pOne[1]) * (pEnd[1] - pOne[1])));
			}
			int brokenModules = morph->getAmountBrokenModules();

			fitness = fitness * pow(0.8, brokenModules);
			pOne.clear();
			pEnd.clear();
			// to do: pEnd - pOne

		//	vector<shared_ptr<ER_Module> > createdModules = morph->getCreatedModules();
		//	vector<float> pos = createdModules[0]->getPosition();
		//	fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
		}
	}

	return fitness;
}

float MoveEnv::updateEnv(MorphologyPointer morph) {
	if (pOne.size() < 1 && simGetSimulationTime() >= timeCheck) {
		int mainHandle = morph->getMainHandle();
		float pos[3];
		simGetObjectPosition(mainHandle, -1, pos);
		pOne.push_back(pos[0]);
		pOne.push_back(pos[1]);
	}
	return 0;
}