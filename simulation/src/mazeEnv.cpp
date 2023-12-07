#include "simulatedER/mazeEnv.h"

#include <boost/algorithm/string.hpp>

using namespace are::sim;

MazeEnv::MazeEnv()
{
    target_position.resize(3);
    final_position.resize(3);
    name = "mazeEnv";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#targetPosition",std::make_shared<const settings::Sequence<double>>(std::vector<double>({0.,0.,0.1})));
    settings::defaults::parameters->emplace("#initPosition",std::make_shared<const settings::Sequence<double>>(std::vector<double>({0.,0.,0.1})));
    settings::defaults::parameters->emplace("#withBeacon",std::make_shared<const settings::Boolean>(true));
    settings::defaults::parameters->emplace("#arenaSize",std::make_shared<const settings::Double>(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",std::make_shared<const settings::Integer>(2));
    settings::defaults::parameters->emplace("#flatFloor",std::make_shared<const settings::Boolean>(true));
    settings::defaults::parameters->emplace("#withTiles",std::make_shared<const settings::Boolean>(true));

}

void MazeEnv::init(){

    VirtualEnvironment::init();

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    std::string scenePath = settings::getParameter<settings::String>(parameters,"#scenePath").value;
    if(verbose){
        int i = 0;
        int handle = 0;
        std::cout << "Loaded scene : " << scenePath << std::endl;
        std::cout << "Objects in the scene : " << std::endl;
        while((handle = simGetObjects(i,sim_handle_all)) >= 0){
            std::cout << simGetObjectAlias(handle,-1) << std::endl;
            i++;
        }
    }

    final_position = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;

    target_position = settings::getParameter<settings::Sequence<double>>(parameters,"#targetPosition").value;

    bool withBeacon = settings::getParameter<settings::Boolean>(parameters,"#withBeacon").value;

    if(withBeacon){
        double bSize[3] = {0.1f,0.1f,0.1f};
        int beacon_handle = simCreatePrimitiveShape(sim_primitiveshape_spheroid,bSize,0); //create a sphere as beacon;
        simSetShapeMass(beacon_handle,0.05f);

        simSetObjectAlias(beacon_handle,"IRBeacon_0",0);
        const double tPos[3] = {static_cast<float>(target_position[0]),
                               static_cast<float>(target_position[1]),
                               static_cast<float>(target_position[2])};

        if(simSetObjectPosition(beacon_handle,-1,tPos) < 0){
            std::cerr << "Set object position failed" << std::endl;
            exit(1);
        }
        simSetObjectSpecialProperty(beacon_handle,sim_objectspecialproperty_detectable_infrared);
        simSetModelProperty(beacon_handle,sim_modelproperty_not_collidable | sim_modelproperty_not_dynamic);
    }

    trajectory.clear();

    bool with_tiles = settings::getParameter<settings::Boolean>(parameters,"#withTiles").value;

    if(with_tiles){
        std::vector<int> th;
        build_tiled_floor(th);
    }


}

std::vector<double> MazeEnv::fitnessFunction(const Individual::Ptr &ind){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> float
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };
    std::vector<double> d(1);
    d[0] = 1 - distance(final_position,target_position)/max_dist;

    for(double& f : d)
        if(std::isnan(f) || std::isinf(f) || f < 0)
            f = 0;
        else if(f > 1) f = 1;

    return d;
}


float MazeEnv::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = morph->getMainHandle();

    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);
//    std::cout << wp.to_string() << std::endl;

    if(wp.is_nan())
        return 1;

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
