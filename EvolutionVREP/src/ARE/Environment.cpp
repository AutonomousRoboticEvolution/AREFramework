#include "ARE/Environment.h"

using namespace are;

void Environment::sceneLoader(int clientID) {
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    std::string scene_path = settings::getParameter<settings::String>(parameters,"#scenePath").value;
    if(sim::loadScene(instance_type,scene_path.c_str(),clientID) < 0)
        std::cerr << "unable to load the scene : " << scene_path << std::endl;
}

void Environment::init(int clID) {
    clientID = clID;
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
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    float time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
	// Sets time step
    sim::setFloatingParameter(instance_type,
                              sim_floatparam_simulation_time_step,
                              time_step,
                              clientID);
}

void Environment::print() {
    std::cout << "Environment is here" << std::endl;
}




