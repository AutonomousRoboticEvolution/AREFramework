#include "hill_climbing.hpp"
#include "simLib/simLib.h"
#include <boost/algorithm/string.hpp>
#include <simulatedER/Morphology.h>

using namespace are;
using namespace are::sim;

HillClimbing::HillClimbing(const settings::ParametersMapPtr& params)
{
    parameters = params;
    final_position.resize(3);
    name = "hill_climbing";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#withTiles",new settings::Boolean(false));

}

void HillClimbing::init(){

    VirtualEnvironment::init();

    bool with_tiles = settings::getParameter<settings::Boolean>(parameters,"#withTiles").value;

    trajectory.clear();

    if(with_tiles){
        std::vector<int> th;
        build_tiled_floor(th);
    }
    /// EB: This shouldn't be here!
    move_counter = 0;
    best_height = 0;
}

std::vector<double> HillClimbing::fitnessFunction(const Individual::Ptr &ind){
    std::vector<double> d(1);
    double max_height = settings::getParameter<settings::Double>(parameters,"#maxHeight").value;
    d[0] = best_height/max_height;
    return d;
}


float HillClimbing::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = std::dynamic_pointer_cast<sim::Morphology>(morph)->getMainHandle();

    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);


    if(wp.position[2] > best_height && simulationTime > 1){
        best_height = wp.position[2];
        std::cout << "best height: " << best_height << std::endl;
    }
            

    if(fabs(final_position[0] - wp.position[0]) > 1e-3 ||
            fabs(final_position[1] - wp.position[1]) > 1e-3)
        move_counter++;

    final_position[0] = static_cast<double>(wp.position[0]);
    final_position[1] = static_cast<double>(wp.position[1]);
    final_position[2] = static_cast<double>(wp.position[2]);

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(simulationTime >= interval*trajectory.size())
        trajectory.push_back(wp);

    return 0;
}

