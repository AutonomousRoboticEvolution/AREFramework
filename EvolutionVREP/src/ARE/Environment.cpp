#include "ARE/Environment.h"

using namespace are;

void Environment::sceneLoader() {
    std::string scene_path = settings::getParameter<settings::String>(parameters,"#scenePath").value;
    if(simLoadScene(scene_path.c_str()) < 0){
        std::cerr << "unable to load the scene : " << scene_path << std::endl;
        exit(1);
    }
}

void Environment::init() {
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value){
        std::cout << "Initialize Environment" << std::endl;
    }
    sceneLoader();
    maxTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    float time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
	// Sets time step
    simSetFloatingParameter(sim_floatparam_simulation_time_step,time_step);
}

void Environment::print() {
    std::cout << "Environment is here" << std::endl;
}




