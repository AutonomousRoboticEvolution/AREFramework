#include "WaterEnv.h"



WaterEnv::WaterEnv()
{
}


WaterEnv::~WaterEnv()
{
}

void WaterEnv::init() {
	simLoadScene("scenes\\WaterER.ttt");
	initialPos.resize(3);
	initialPos[0] = 0.0;
	initialPos[1] = 0.0;
	initialPos[2] = 0.1;
	cout << "environment Initialized" << endl;
	simSetInt32Parameter(sim_intparam_dynamic_step_divider, 25);
}

float WaterEnv::fitnessFunction(MorphologyPointer morph) {
	float fitness = 0;

	if (morph->modular == false) {
		cout << "getting main handle" << endl;
		int mainHandle = morph->getMainHandle();
		float pos[3];
		simGetObjectPosition(mainHandle, -1, pos);
		fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]);
		//	fitness = 0; // no fixed morphology that can absorb light
	}
	else {
		int mainHandle = morph->getMainHandle();
		float pos[3];
		simGetObjectPosition(mainHandle, -1, pos);
		fitness = sqrtf(pos[0] * pos[0]) + sqrtf(pos[1] * pos[1]) + sqrtf(pos[2] * pos[2]);

	}
	return fitness;
}

float WaterEnv::updateEnv(MorphologyPointer morph) {
	// apply drag forces to each object, possibly object facet?
	float fitness = 0;
	if (morph->modular = false) {
		fitness = 0; // no fixed morphology that can absorb light
	}
	else {
		vector<shared_ptr<ER_Module> > createdModules = morph->getCreatedModules();
		for (int i = 0; i < createdModules.size(); i++) {
			for (int j = 0; j < createdModules[i]->objectHandles.size(); j++) {
				float lv[3];

				float av[3];
				simGetVelocity(createdModules[i]->objectHandles[j], lv, av);


				float force[3] = { 0, 0, 0 };
				float torque[3] = { 0,0,0 };
				if (av[0] < 100 && av[0] > -100 && av[1] < 100 && av[1] > -100 && av[2] < 100 && av[2] > -100) {
					torque[0] = (-0.002 * av[0]);
					torque[1] = (-0.002 * av[1]);
					torque[2] = (-0.002 * av[2]);
				}
				for (int k = 0; k < 3; k++) {
					float F; // force
					float p = 1; // density
					float C = 0.5; // facet's drag
					float A = 0.5; // facet's area
					float v = 0;
					if (lv[k] < 100 && lv[k] > 100) {
						v = lv[k]; // facet's normal speed
					}
					//			float m;
					//			simGetObjectFloatParameter(createdModules[i]->objectHandles[j], 3005, &m);
					force[k] = -0.5 * p * C * A * v;
					if (force[k] > 0.001) {
						cout << "nok" << endl;
					}
					if (force[k] > 100) {
						break;
					}
				}

				float position[3];
				simGetObjectPosition(createdModules[i]->objectHandles[j], -1, position);
				//	simAddForce(createdModules[i]->objectHandles[j], position, force);
				simAddForceAndTorque(createdModules[i]->objectHandles[j], force, torque);
			}
		}
	}
	
	return 0;
}