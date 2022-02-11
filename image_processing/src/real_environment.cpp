#include "image_processing/real_environment.hpp"

using namespace are;

RealEnvironment::RealEnvironment():
    Environment(),
    robot_pos_subs(context, ZMQ_REQ),
    tags_pos_subs(context, ZMQ_SUB){

    current_position.resize(2);
    beacon_position.resize(2);
    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#target",new settings::Sequence<double>({0.,0.,0.12}));
}

void RealEnvironment::init(){

     std::string pipe = settings::getParameter<settings::String>(parameters,"#cameraPipe").value;

     //** Subscribers to camera topics
     robot_pos_subs.connect(pipe);
     robot_pos_subs.set(zmq::sockopt::subscribe, "Robot:");
     tags_pos_subs.connect(pipe);
     tags_pos_subs.set(zmq::sockopt::subscribe, "Tags:");
     //*/

     grid_zone = Eigen::MatrixXi::Zero(8,8);

     std::vector<int> colour_bnd = settings::getParameter<settings::Sequence<int>>(parameters,"#colourBoundaries").value;
     colour_range.first = cv::Scalar(colour_bnd[0],colour_bnd[2],colour_bnd[4]);
     colour_range.second = cv::Scalar(colour_bnd[1],colour_bnd[3],colour_bnd[5]);


     target_position = settings::getParameter<settings::Sequence<double>>(parameters,"#target").value;

}

std::vector<double> RealEnvironment::fitnessFunction(const Individual::Ptr &ind){
    int env_type = are::settings::getParameter<are::settings::Integer>(parameters,"#envType").value;

    if(env_type == 0)
        return fit_targeted_locomotion();
    else if(env_type == 1)
        return fit_exploration();
    else if(env_type == 2)
        return fit_foraging();

    std::cerr << "Unknown type of environment type : " << env_type << std::endl;
    return {0};

}

std::vector<double> RealEnvironment::fit_targeted_locomotion(){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
            (a[1] - b[1])*(a[1] - b[1]));
    };
    std::vector<double> d(1);
    d[0] = 1 - distance(current_position,target_position)/max_dist;
    final_position = current_position;
    return d;
}

std::vector<double> RealEnvironment::fit_exploration(){
    return {static_cast<double>(grid_zone.sum())/64.f};
}

std::vector<double> RealEnvironment::fit_foraging(){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                (a[1] - b[1])*(a[1] - b[1]) +
                (a[2] - b[2])*(a[2] - b[2]));
    };
    std::vector<double> d(1);
    // Distance between beacon and target
    //d[0] = 1 - distance(final_position,target_position)/max_dist;
    d[0] = 1 - ((distance(current_position,target_position)/max_dist) *
                + (distance(beacon_position,target_position)/max_dist)) * 0.5;

    for(double& f : d)
        if(std::isnan(f) || std::isinf(f) || f < 0)
            f = 0;
        else if(f > 1) f = 1;

    return d;
}

void RealEnvironment::update_info(double time){
    std::string robot_position;
    phy::send_string(robot_position,"Robot:position",robot_pos_subs,"Robot:");

    std::stringstream sstr(robot_position);
    cv::Point2f pixel_coord;
    std::string topic;
    char par1,par2,comma;
    sstr >> topic >> par1 >> pixel_coord.x >> comma >> pixel_coord.y >> par2;
    std::cout << "From topic " << topic << " position " << pixel_coord << std::endl;
    if(pixel_coord.x == -999 || pixel_coord.y == -999)
        return;
    cv::Point2f world_coord = image_proc::convert_pixel_to_world_frame(pixel_coord,parameters);
    current_position = {world_coord.x,world_coord.y};

    std::pair<int,int> indexes = real_coordinate_to_matrix_index(current_position);
    grid_zone(indexes.first,indexes.second) = 1;

    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(time >= interval*trajectory.size()){
        waypoint wp;
        wp.position[0] = current_position[0];
        wp.position[1] = current_position[1];
        wp.position[2] = 0;
        wp.orientation[0] = 0;
        wp.orientation[1] = 0;
        wp.orientation[2] = 0;

        trajectory.push_back(wp);
    }

    final_position = current_position;

    for(const auto &val : current_position)
        std::cout << val << ",";
}


std::pair<int,int> RealEnvironment::real_coordinate_to_matrix_index(const std::vector<double> &pos){
    std::pair<int,int> indexes;

    indexes.first = std::trunc(pos[0]/0.25 + 4);
    indexes.second = std::trunc(pos[1]/0.25 + 4);
    if(indexes.first == 8)
        indexes.first = 7;
    if(indexes.second == 8)
        indexes.second = 7;
    return indexes;
}
