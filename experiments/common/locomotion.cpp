#include "locomotion.hpp"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib/simLib.h"
#endif
#include <boost/algorithm/string.hpp>
#include <simulatedER/Morphology.h>

using namespace are;
using namespace are::sim;

Locomotion::Locomotion(const settings::ParametersMapPtr& params)
{
    parameters = params;
    final_position.resize(3);
    name = "locomotion";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#withTiles",new settings::Boolean(false));

}

void Locomotion::init(){

    VirtualEnvironment::init();

    bool with_tiles = settings::getParameter<settings::Boolean>(parameters,"#withTiles").value;

    trajectory.clear();

    if(with_tiles){
        std::vector<int> th;
        build_tiled_floor(th);
    }
    /// EB: This shouldn't be here!
    move_counter = 0;
}

std::vector<double> Locomotion::fitnessFunction(const Individual::Ptr &ind){

    std::vector<double> d(1);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };


    std::vector<double> init_position = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    double max_distance = settings::getParameter<settings::Double>(parameters,"#maxDistance").value;
    double distance_covered = distance(final_position,init_position);

     d[0] = distance_covered/max_distance;

    return d;
}


float Locomotion::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = std::dynamic_pointer_cast<sim::Morphology>(morph)->getMainHandle();

    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);


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

