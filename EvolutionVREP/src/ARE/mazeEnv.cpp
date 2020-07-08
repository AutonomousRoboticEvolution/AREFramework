#include "ARE/mazeEnv.h"
#include "v_repLib.h"

#include <boost/algorithm/string.hpp>

using namespace are;

MazeEnv::MazeEnv()
{
    target_position.resize(3);
    final_position.resize(3);
    name = "mazeEnv";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#target_x",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_y",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_z",new settings::Double(0.05));
    settings::defaults::parameters->emplace("#withBeacon",new settings::Boolean(false));
    settings::defaults::parameters->emplace("#arenaSize",new settings::Double(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));

}

void MazeEnv::init(){

    Environment::init();

       target_position = {settings::getParameter<settings::Double>(parameters,"#target_x").value,
                       settings::getParameter<settings::Double>(parameters,"#target_y").value,
                       settings::getParameter<settings::Double>(parameters,"#target_z").value};

    bool withBeacon = settings::getParameter<settings::Boolean>(parameters,"#withBeacon").value;
    if(withBeacon){
        float bSize[3] = {0.1f,0.1f,0.1f};
        int beacon_handle = simCreatePureShape(1,0,bSize,0.05f,nullptr); //create a sphere as beacon;
        float tPos[3] = {static_cast<float>(target_position[0]),
                         static_cast<float>(target_position[1]),
                         static_cast<float>(0.05f)};
        simSetObjectName(beacon_handle,"IRBeacon_0");
        simSetObjectPosition(beacon_handle,-1,tPos);
        simSetObjectSpecialProperty(beacon_handle,sim_objectspecialproperty_detectable_infrared);
        simSetModelProperty(beacon_handle,sim_modelproperty_not_collidable | sim_modelproperty_not_dynamic);
    }

    trajectory.clear();

}

std::vector<double> MazeEnv::fitnessFunction(const Individual::Ptr &ind){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
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
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = morph->getMainHandle();


    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);

    if(fabs(final_position[0] - wp.position[0]) > 1e-1 ||
            fabs(final_position[1] - wp.position[1]) > 1e-1 ||
            fabs(final_position[2] - wp.position[2]) > 1e-1)
        move_counter++;

    final_position[0] = static_cast<double>(wp.position[0]);
    final_position[1] = static_cast<double>(wp.position[1]);
    final_position[2] = static_cast<double>(wp.position[2]);

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(simulationTime >= interval*trajectory.size())
        trajectory.push_back(wp);

    return 0;
}
