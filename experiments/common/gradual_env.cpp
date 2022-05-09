#include "gradual_env.hpp"
#include <boost/algorithm/string.hpp>

using namespace are::sim;

GradualEnvironment::GradualEnvironment(const settings::ParametersMapPtr& params)
{
    parameters = params;

    final_position.resize(3);
    init_position.resize(3);
    name = "gradual_tasks";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#arenaSize",new settings::Double(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));

    std::string filename = settings::getParameter<settings::String>(params,"#envListFile").value;
    load_environments_list(filename,environments_info);
    trajectories.resize(environments_info.size());

}

void GradualEnvironment::init(){


    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if(simLoadScene(environments_info[current_scene].scene_path.c_str()) < 0){
        std::cerr << "unable to load the scene : " << environments_info[current_scene].scene_path << std::endl;
        exit(1);
    }

    if(verbose){
        int i = 0;
        int handle = 0;
        std::cout << "Loaded scene : " << environments_info[current_scene].scene_path << std::endl;
        std::cout << "Objects in the scene : " << std::endl;
        while((handle = simGetObjects(i,sim_handle_all)) >= 0){
            std::cout << simGetObjectName(handle) << std::endl;
            i++;
        }
    }
    init_position = environments_info[current_scene].init_position;
    if(environments_info[current_scene].fitness_fct == TARGET ||
            environments_info[current_scene].fitness_fct == FORAGING)
        target_position = environments_info[current_scene].target_position;
    final_position = init_position;

    trajectory.clear();

    grid_zone = Eigen::MatrixXi::Zero(8,8);
    number_of_collisions = 0;

    bool with_tiles = settings::getParameter<settings::Boolean>(parameters,"#withTiles").value;

    if(with_tiles){
        std::vector<int> th;
        build_tiled_floor(th);
    }
}

std::vector<double> GradualEnvironment::fitnessFunction(const Individual::Ptr &ind){
    if(_fitness_fcts == EXPLORATION)
        return _exploration();
    else if(_fitness_fcts == TARGET)
        return _targeted_locomotion();
    else if(_fitness_fcts == FORAGING)
        return _foraging();
    else{
        std::cerr << "gradual tasks environment error: unknown fitness function.";
        return std::vector<double>(0);
    }
}

std::pair<int,int> GradualEnvironment::real_coordinate_to_matrix_index(const std::vector<double> &pos){
    std::pair<int,int> indexes;

    indexes.first = std::trunc(pos[0]/0.25 + 4);
    indexes.second = std::trunc(pos[1]/0.25 + 4);
    if(indexes.first == 8)
        indexes.first = 7;
    if(indexes.second == 8)
        indexes.second = 7;
    return indexes;
}


float GradualEnvironment::updateEnv(float simulationTime, const Morphology::Ptr &morph){

    auto L1 = [](std::pair<int,int> p1, std::pair<int,int> p2)-> int {
        return abs(p1.first - p2.first) + abs(p1.second - p2.second);
    };

    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = morph->getMainHandle();

    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);


    if(wp.is_nan())
        return 1;

    if(fabs(final_position[0] - wp.position[0]) > 1e-1 ||
            fabs(final_position[1] - wp.position[1]) > 1e-1 ||
            fabs(final_position[2] - wp.position[2]) > 1e-1)
        move_counter++;

    final_position[0] = static_cast<double>(wp.position[0]);
    final_position[1] = static_cast<double>(wp.position[1]);
    final_position[2] = static_cast<double>(wp.position[2]);

    std::pair<int,int> init_indexes = real_coordinate_to_matrix_index(init_position);
    std::pair<int,int> indexes = real_coordinate_to_matrix_index(final_position);
    assert(indexes.first >= 0 && indexes.first < 8 && indexes.second >= 0 && indexes.second < 8);
    grid_zone(indexes.first,indexes.second) = L1(init_indexes,indexes);

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(simulationTime >= interval*trajectory.size())
        trajectory.push_back(wp);
    else if(simulationTime >= evalTime){
        trajectory.push_back(wp);
        trajectories[current_scene] = trajectory;
    }

    return 0;
}

void GradualEnvironment::build_tiled_floor(std::vector<int> &tiles_handles){
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
                height = (i+j)%2 == 0 ? -0.004 : 0.006;

//                height = randNum->randFloat(-0.005,-0.001);
            }
            float pos[3] = {starting_pos[0] + i*tile_increment,starting_pos[1] + j*tile_increment,height};
            simSetObjectPosition(tiles_handles.back(),-1,pos);
            simSetEngineFloatParameter(sim_bullet_body_friction,tiles_handles.back(),nullptr,1000);//randNum->randFloat(0,1000));
            simSetObjectSpecialProperty(tiles_handles.back(),sim_objectspecialproperty_detectable_ultrasonic);
            simSetModelProperty(tiles_handles.back(), sim_modelproperty_not_dynamic);
        }
    }
}

void GradualEnvironment::load_environments_list(const std::string &file_name, std::vector<env_t> &env_info){
    std::ifstream ifs(file_name);
    if(!ifs){
        std::cerr << "Unable to open file : " << file_name << std::endl;
        return;
    }

    std::string models_folder = settings::getParameter<settings::String>(parameters,"#modelsPath").value;

    std::vector<std::string> split;
    std::vector<std::string> split2;
    env_t env;
    for(std::string line; std::getline(ifs,line);){
        misc::split_line(line,";",split);
        env.scene_path = models_folder + "/scenes/" + split[0];
        misc::split_line(split[2],",",split2);
        env.init_position = {std::stod(split2[0]),std::stod(split2[1]),std::stod(split2[2])};
        if(split[1] == "exploration")
            env.fitness_fct = EXPLORATION;
        else if(split[1] == "target"){
            env.fitness_fct = TARGET;
            misc::split_line(split[3],",",split2);
            env.target_position = {std::stod(split2[0]),std::stod(split2[1]),std::stod(split2[2])};
        }else if(split[1] == "foraging"){
            env.fitness_fct = FORAGING;
            misc::split_line(split[3],",",split2);
            env.target_position = {std::stod(split2[0]),std::stod(split2[1]),std::stod(split2[2])};
        }
        env_info.push_back(env);
    }
}

std::vector<double> GradualEnvironment::_exploration(){
    auto max_fitness = [](int max,std::pair<int,int> init)->int{
        int sum = 0;
        for(int i = 0; i < max; i++)
            for(int j = 0; j < max; j++)
                sum += abs(i - init.first) + abs(j - init.second);
        return sum;
    };

    return {static_cast<double>(grid_zone.sum())/static_cast<double>(max_fitness(8,real_coordinate_to_matrix_index(init_position)))};
}

std::vector<double> GradualEnvironment::_targeted_locomotion(){
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
