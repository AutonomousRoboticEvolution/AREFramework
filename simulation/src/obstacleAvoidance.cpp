#include "simulatedER/obstacleAvoidance.hpp"

#include <boost/algorithm/string.hpp>

using namespace are::sim;

ObstacleAvoidance::ObstacleAvoidance(const settings::ParametersMapPtr& params)
{
    parameters = params;

    final_position.resize(3);
    name = "obstacle_avoidance";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#arenaSize",std::make_shared<settings::Double>(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",std::make_shared<settings::Integer>(2));
    settings::defaults::parameters->emplace("#flatFloor",std::make_shared<settings::Boolean>(true));
}

void ObstacleAvoidance::init(){

    VirtualEnvironment::init();

    //bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    std::string scenePath = settings::getParameter<settings::String>(parameters,"#scenePath").value;

    final_position = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;

    trajectory.clear();

    grid_zone = Eigen::MatrixXi::Zero(8,8);
    number_of_collisions = 0;


    bool with_tiles = settings::getParameter<settings::Boolean>(parameters,"#withTiles").value;

    if(with_tiles){
        std::vector<int> th;
        build_tiled_floor(th);
    }


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

    waypoint wp;
    std::vector<double> position = morph->get_position();
    std::vector<double> orientation = morph->get_orientation();
    wp.position[0] = position[0];
    wp.position[1] = position[1];
    wp.position[2] = position[2];
    wp.orientation[0] = orientation[0];
    wp.orientation[1] = orientation[1];
    wp.orientation[2] = orientation[2];


//    int obst_handle;
//    int coll;
//    for(int i = 0; i < 11; i++){
//        std::stringstream sstr;
//        sstr << "Obstacle_" << i;
//        obst_handle = simGetObjectHandle(sstr.str().c_str());
//        int nbr_handles;
//        int* handles = simGetObjectsInTree(morphHandle,sim_handle_all,0,&nbr_handles);
//        coll = simCheckCollision(morphHandle,obst_handle);
//        assert(coll >= 0);
//        for(int j = 0; j < nbr_handles; j++)
//            coll += simCheckCollision(handles[j],obst_handle);
//        if(coll > 0 && number_of_collisions < 64)
//            number_of_collisions+=1;
//    }
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

