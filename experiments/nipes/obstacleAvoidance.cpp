#include "obstacleAvoidance.hpp"

#include <boost/algorithm/string.hpp>

using namespace are::sim;

ObstacleAvoidance::ObstacleAvoidance(const settings::ParametersMapPtr& params)
{
    parameters = params;

    final_position.resize(3);
    name = "obstacle_avoidance";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#arenaSize",new settings::Double(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));
}

void ObstacleAvoidance::init(){

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

    trajectory.clear();

    grid_zone = Eigen::MatrixXi::Zero(8,8);
    number_of_collisions = 0;

    std::vector<int> th;
    build_tiled_floor(th);


}

std::vector<double> ObstacleAvoidance::fitnessFunction(const Individual::Ptr &ind){
   // if(number_of_collisions == 0)
        return {static_cast<double>(grid_zone.sum())/64.f};
    //return {(static_cast<double>(grid_zone.sum())/static_cast<double>(number_of_collisions))/64.f};
}

std::pair<int,int> ObstacleAvoidance::real_coordinate_to_matrix_index(const std::vector<double> &pos){
    std::pair<int,int> indexes;

    indexes.first = std::trunc(pos[0]/0.25 + 4);
    indexes.second = std::trunc(pos[1]/0.25 + 4);
    if(indexes.first == 8)
        indexes.first = 7;
    if(indexes.second == 8)
        indexes.second = 7;
    return indexes;
}


float ObstacleAvoidance::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = morph->getMainHandle();

    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);

    int obst_handle;
    int coll;
    for(int i = 0; i < 11; i++){
        std::stringstream sstr;
        sstr << "Obstacle_" << i;
        obst_handle = simGetObjectHandle(sstr.str().c_str());
        int nbr_handles;
        int* handles = simGetObjectsInTree(morphHandle,sim_handle_all,0,&nbr_handles);
        coll = simCheckCollision(morphHandle,obst_handle);
        assert(coll >= 0);
        for(int j = 0; j < nbr_handles; j++)
            coll += simCheckCollision(handles[j],obst_handle);
        if(coll > 0 && number_of_collisions < 64)
            number_of_collisions+=1;
    }
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

    std::pair<int,int> indexes = real_coordinate_to_matrix_index(final_position);
    grid_zone(indexes.first,indexes.second) = 1;

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(simulationTime >= interval*trajectory.size())
        trajectory.push_back(wp);
    else if(simulationTime >= evalTime)
        trajectory.push_back(wp);

    return 0;
}

void ObstacleAvoidance::build_tiled_floor(std::vector<int> &tiles_handles){
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


