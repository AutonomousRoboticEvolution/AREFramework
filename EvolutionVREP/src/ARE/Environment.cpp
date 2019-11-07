#include "ARE/Environment.h"

using namespace are;

Environment::Environment()
{	
}

Environment::~Environment()
{

}

void Environment::sceneLoader() {
	simCloseScene();
    simLoadScene(settings->scene_path);//todo add in settings
//    "scenes/DefaultERLight.ttt"
}

void Environment::init() {
    if(settings->verbose){
        std::cout << "Initialize Environment" << std::endl;
    }
    sceneLoader();
    // Every time the environment is created, it removes all objects and creates them again
	envObjectHandles.clear();
	initialPos.resize(3);
	initialPos[0] = 0.0;
	initialPos[1] = 0.0;
	initialPos[2] = 0.1;
	// Sets time step
    simSetFloatingParameter(sim_floatparam_simulation_time_step, 0.050);
}

void Environment::print() {
    std::cout << "Environment is here" << std::endl;
}

//float Environment::fitnessFunction(Individual::Ptr morph) {
//	float fitness = 0.0;
//    std::cout << "empty morphology" << std::endl;
//	return fitness;
//}


