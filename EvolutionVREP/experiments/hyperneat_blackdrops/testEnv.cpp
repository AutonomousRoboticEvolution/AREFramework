#include "testEnv.h"
#include "v_repLib.h"

using namespace are;

double TestEnv::fitnessFunction(const Individual::Ptr &ind){
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };
    double d = distance(initial_position,final_position);
    ind->setFitness(d);
    return d;
}

float TestEnv::updateEnv(float simulationTime, const Morphology::Ptr &morph){

    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    float timeStep = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    int morphHandle = morph->getMainHandle();

    float pos[3];
    sim::getObjectPosition(instance_type, morphHandle, -1, pos, clientID);

    if(simulationTime < timeStep*2)
    {
        initial_position[0] = static_cast<double>(pos[0]);
        initial_position[1] = static_cast<double>(pos[1]);
        initial_position[2] = static_cast<double>(pos[2]);
    }
    else
    {
        final_position[0] = static_cast<double>(pos[0]);
        final_position[1] = static_cast<double>(pos[1]);
        final_position[2] = static_cast<double>(pos[2]);
    }

    return 0;
}
