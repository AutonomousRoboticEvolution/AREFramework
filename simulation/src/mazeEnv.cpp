#include "simulatedER/mazeEnv.h"


#include <boost/algorithm/string.hpp>

using namespace are::sim;
// make a local copy of mazeEnv.cpp in the morphneuro experiments and leave the old one
// (remove function load_target_positions (line 168-183) and line 102-107) unchanged
//  mv .h, .c to morphneuro/ and remove these lines
MazeEnv::MazeEnv()
{
    target_position.resize(3);
    final_position.resize(3);
    name = "mazeEnv";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#target_x",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_y",new settings::Double(0.));
    settings::defaults::parameters->emplace("#target_z",new settings::Double(0.05));


    settings::defaults::parameters->emplace("#withBeacon",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#arenaSize",new settings::Double(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));

    load_target_positions();
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
            std::cout << simGetObjectName(handle) << std::endl;
            i++;
        }
    }

    bool multi_target = settings::getParameter<settings::Boolean>(parameters,"#isMultiTarget").value;
    if(multi_target){
        target_position = target_position_all[current_target];
    }else{
        target_position = {settings::getParameter<settings::Double>(parameters,"#target_x").value,
                           settings::getParameter<settings::Double>(parameters,"#target_y").value,
                           settings::getParameter<settings::Double>(parameters,"#target_z").value};
    }


    bool withBeacon = settings::getParameter<settings::Boolean>(parameters,"#withBeacon").value;

    if(withBeacon){
        float bSize[3] = {0.1f,0.1f,0.1f};
        int beacon_handle = simCreatePureShape(1,0,bSize,0.05f,nullptr); //create a sphere as beacon;

        simSetObjectName(beacon_handle,"IRBeacon_0");
        const float tPos[3] = {static_cast<float>(target_position[0]),
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

    std::vector<int> th;
    build_tiled_floor(th);
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
    target_position = target_position_all[current_target];
    d[0] = 1 - distance(final_position,target_position)/max_dist;

    for(double& f : d)
        if(std::isnan(f) || std::isinf(f) || f < 0)
            f = 0;
        else if(f > 1) f = 1;

    bool multi_target = settings::getParameter<settings::Boolean>(parameters,"#isMultiTarget").value;
    if(multi_target){
        current_target+=1;
        if(current_target >= target_position_all.size())
            current_target=0;
    }
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
    else if(simulationTime >= evalTime)
        trajectory.push_back(wp);

    return 0;
}

void MazeEnv::build_tiled_floor(std::vector<int> &tiles_handles){
    bool flatFloor = settings::getParameter<settings::Boolean>(parameters,"#flatFloor").value;

    float tile_size[3] = {0.249f,0.249f,0.01f};
    float tile_increment = 0.25;
    float starting_pos[3] = {-0.875f,-0.875f,0.005f};
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            tiles_handles.push_back(simCreatePureShape(0,8,tile_size,0.05f,nullptr));
            std::stringstream name;
            name << "tile_" << i << j;
            simSetObjectName(tiles_handles.back(),name.str().c_str());
            float height = -0.004;
            if(!flatFloor){
                height = randNum->randFloat(-0.005,-0.001);
            }
            float pos[3] = {starting_pos[0] + i*tile_increment,starting_pos[1] + j*tile_increment,height};
            simSetObjectPosition(tiles_handles.back(),-1,pos);
            simSetEngineFloatParameter(sim_bullet_body_friction,tiles_handles.back(),nullptr,1000);//randNum->randFloat(0,1000));
            simSetObjectSpecialProperty(tiles_handles.back(),sim_objectspecialproperty_detectable_ultrasonic);
            simSetModelProperty(tiles_handles.back(), sim_modelproperty_not_dynamic);
        }
    }
}

void MazeEnv::load_target_positions(){
//    double pos1,pos2,pos3;
    std::vector<double> pos1;
    std::vector<double> pos2;
    std::vector<double> pos3;
    pos1.resize(3);
    pos2.resize(3);
    pos3.resize(3);
    pos1 = {0.75,0.75,0.12};
    pos2 = {-0.75,0.75,0.12};
    pos3 = {-0.75,-0.75,0.12};
    target_position_all.push_back(pos1);
    target_position_all.push_back(pos2);
    target_position_all.push_back(pos3);
}