#include "ARE/mazeEnv.h"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include <boost/algorithm/string.hpp>

using namespace are;

MazeEnv::MazeEnv()
{
    target_position.resize(3);
    final_position.resize(3);
    start_position.resize(3);
    name = "mazeEnv";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#target_x",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_y",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_z",new settings::Double(0.05));
    settings::defaults::parameters->emplace("#withBeacon",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#arenaSize",new settings::Double(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#start_x",new settings::Double(0.));
    settings::defaults::parameters->emplace("#start_y",new settings::Double(0.));
    settings::defaults::parameters->emplace("#start_z",new settings::Double(0.05));

}

void MazeEnv::init(){

    Environment::init();

       target_position = {settings::getParameter<settings::Double>(parameters,"#target_x").value,
                       settings::getParameter<settings::Double>(parameters,"#target_y").value,
                       settings::getParameter<settings::Double>(parameters,"#target_z").value};
       start_position = {settings::getParameter<settings::Double>(parameters,"#start_x").value,
                         settings::getParameter<settings::Double>(parameters,"#start_y").value,
                         settings::getParameter<settings::Double>(parameters,"#start_z").value};

    bool withBeacon = settings::getParameter<settings::Boolean>(parameters,"#withBeacon").value;
    bool flatFloor = settings::getParameter<settings::Boolean>(parameters,"#flatFloor").value;

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

    if(!flatFloor){
        std::vector<int> th;
        build_tiled_floor(th);
    }

}

//std::vector<double> MazeEnv::fitnessFunction(const Individual::Ptr &ind){
//    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
//    double max_dist = sqrt(2*arena_size*arena_size);
//    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
//    {
//        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
//                         (a[1] - b[1])*(a[1] - b[1]) +
//                         (a[2] - b[2])*(a[2] - b[2]));
//    };
//    std::vector<double> d(1);
//    d[0] = 1 - distance(final_position,target_position)/2.83f;
////// Locomotion
////    d[0] = distance(final_position,start_position)/2.83f;
//    std::cout << "Locomotion Fitness: " << d[0] << std::endl;
//    return d;
//}

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
    d[0] = 1 - distance(final_position,target_position)/max_dist;

//    if (d[0] < 0){
//        d[0] = 0;
//    }else if (d[0] > 1){
//        d[0] = 1;
//    }

//    for(double& f : d)
//        if(std::isnan(f) || std::isinf(f))
//            f = 0;

//    assert(d[0] <= 1 && d[0] >= 0);

    return d;
}

float MazeEnv::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;   //used for storing trajectory
    int morphHandle = morph->getMainHandle();


    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);

    /// EB: This is for the journal.
    if(fabs(final_position[0] - wp.position[0]) > 1e-3 ||
       fabs(final_position[1] - wp.position[1]) > 1e-3)
         move_counter++;

//    if(fabs(final_position[0] - wp.position[0]) > 1e-1 ||
//            fabs(final_position[1] - wp.position[1]) > 1e-1 ||
//            fabs(final_position[2] - wp.position[2]) > 1e-1)
//        move_counter++;

    final_position[0] = static_cast<double>(wp.position[0]);
    final_position[1] = static_cast<double>(wp.position[1]);
    final_position[2] = static_cast<double>(wp.position[2]);

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(simulationTime >= interval*trajectory.size())
        trajectory.push_back(wp);

    /// \todo EB: This is temporal code for the experiment in the journal October
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };
    std::vector<double> d(1);
    d[0] = 1 - distance(final_position,target_position)/2.83f;

    if(d[0] > 0.95) {
        simStopSimulation();
    }
    if(simGetSimulationTime() > 10.0 & move_counter < 10) {
        simStopSimulation();
    }

    return 0;
}

void MazeEnv::build_tiled_floor(std::vector<int> &tiles_handles){
    float tile_size[3] = {0.25f,0.25f,0.01f};
    float starting_pos[3] = {-0.875f,-0.875f,0.005f};
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            tiles_handles.push_back(simCreatePureShape(0,8,tile_size,0.05f,nullptr));
            std::stringstream name;
            name << "tile_" << i << j;
            simSetObjectName(tiles_handles.back(),name.str().c_str());
            float pos[3] = {starting_pos[0] + i*tile_size[0],starting_pos[1] + j*tile_size[1],randNum->randFloat(-0.005,-0.001)};
            simSetObjectPosition(tiles_handles.back(),-1,pos);
            simSetEngineFloatParameter(sim_bullet_body_friction,tiles_handles.back(),nullptr,randNum->randFloat(0,1));
            simSetObjectSpecialProperty(tiles_handles.back(),sim_objectspecialproperty_detectable_ultrasonic);
            simSetModelProperty(tiles_handles.back(), sim_modelproperty_not_dynamic);
        }
    }
}
