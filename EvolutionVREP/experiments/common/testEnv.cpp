#include "testEnv.h"
#if defined(VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

using namespace are;

std::vector<double> TestEnv::fitnessFunction(const Individual::Ptr &ind){
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };
    // get target position from parameters
    float target_x = settings::getParameter<settings::Float>(parameters,"#target_x").value;
    float target_y = settings::getParameter<settings::Float>(parameters,"#target_y").value;
    float target_z = settings::getParameter<settings::Float>(parameters,"#target_z").value;
    std::vector<double> destination_position;
    destination_position.push_back(static_cast<double>(target_x));
    destination_position.push_back(static_cast<double>(target_y));
    destination_position.push_back(static_cast<double>(target_z));

    std::vector<double> d(1);
    //d[0] = 1.0 - distance(destination_position, final_position)/2.83;
    d[0] = distance(initial_position, final_position);
    ind->setObjectives(d);
    return d;
}

float TestEnv::updateEnv(float simulationTime, const Morphology::Ptr &morph){

    float timeStep = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    int morphHandle = morph->getMainHandle();

    float pos[3];
    simGetObjectPosition(morphHandle, -1, pos);

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
