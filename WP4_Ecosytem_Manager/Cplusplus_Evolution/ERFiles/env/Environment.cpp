#include "Environment.h"

Environment::Environment()
{	
}

Environment::~Environment()
{

}

void Environment::sceneLoader() {
	simCloseScene();
	simLoadScene("scenes/DefaultERLight.ttt");
}

void Environment::init() {
	envObjectHandles.clear(); // every time the environment is created, it removes all objects and creates them again
	simLoadScene("scenes/DefaultERLight.ttt");

	//	simLoadScene("scenes/Cat_Scene2.ttt");

	initialPos.resize(3);
	initialPos[0] = 0.0;
	initialPos[1] = 0.0;
	initialPos[2] = 0.1;
	cout << "environment Initialized" << endl;
	maxTime = 10.0; 
	float ts[1];
	int a = simSetFloatingParameter(sim_floatparam_simulation_time_step, 0.050);
	int b = simGetFloatingParameter(sim_floatparam_simulation_time_step, ts);
	int f = 0;
//	simSetSimulationTimeStep(50);
//	int a[1];
//	simGetInt32Parameter(sim_intparam_dynamic_step_divider, a);
}

void Environment::print() {
	cout << "Environment is here" << endl; 
}

float Environment::fitnessFunction(MorphologyPointer morph) {
	float fitness = 0.0;
	cout << "empty morphology" << endl;
	return fitness;
}