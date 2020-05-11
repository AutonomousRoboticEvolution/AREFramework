#include "mazeEnv.h"
#include "v_repLib.h"

#include <boost/algorithm/string.hpp>

using namespace are;

void MazeEnv::init(){

    randNum.setSeed(rand());

    Environment::init();


    target_position = {settings::getParameter<settings::Float>(parameters,"#target_x").value,
                       settings::getParameter<settings::Float>(parameters,"#target_y").value,
                       settings::getParameter<settings::Float>(parameters,"#target_z").value};

}

std::vector<double> MazeEnv::fitnessFunction(const Individual::Ptr &ind){
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };
    std::vector<double> d(1);
    d[0] = 1 - distance(final_position,target_position)/2.83f;

    return d;
}


float MazeEnv::updateEnv(float simulationTime, const Morphology::Ptr &morph){

    float timeStep = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    int morphHandle = morph->getMainHandle();

    float pos[3];
    simGetObjectPosition(morphHandle, -1, pos);


    final_position[0] = static_cast<double>(pos[0]);
    final_position[1] = static_cast<double>(pos[1]);
    final_position[2] = static_cast<double>(pos[2]);


    return 0;
}
