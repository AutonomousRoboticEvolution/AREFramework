#include "ARE/Environment.h"

using namespace are;

void Environment::sceneLoader() {
	simCloseScene();
    simLoadScene(settings::getParameter<settings::String>(parameters,"#scenePath").value.c_str());
//    "scenes/DefaultERLight.ttt"
}

void Environment::init() {
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value){
        std::cout << "Initialize Environment" << std::endl;
    }
    sceneLoader();
    maxTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
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




