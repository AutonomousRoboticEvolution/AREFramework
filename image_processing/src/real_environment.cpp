#include "image_processing/real_environment.hpp"

using namespace are;

RealEnvironment::RealEnvironment():
    Environment(),
    zmq_tracking_camera_requester_socket(context, ZMQ_REQ)
{

    current_position.resize(2);
    beacon_position.resize(2);
    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#target",new settings::Sequence<double>({0.,0.,0.12}));
    settings::defaults::parameters->emplace("#initPosition",new settings::Sequence<double>({0.,0.,0.12}));
}

void RealEnvironment::init(){

    //start zmq
    std::string pipe = settings::getParameter<settings::String>(parameters,"#cameraPipe").value;
    zmq_tracking_camera_requester_socket.connect( pipe );

    // initialise the variables for robot and beacon location to some sensible defaults, in case they cannot be seen in the first frame:
    // TODO make these parameters?
    current_position = {0,0};
    beacon_position = {0,0};

    grid_zone = Eigen::MatrixXi::Zero(grid_zone_size,grid_zone_size);

    target_position = settings::getParameter<settings::Sequence<double>>(parameters,"#target").value;
    initial_position = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;

    // reset frames counters
    number_of_frames_where_robot_was_seen = 0;
    number_of_frames_where_barrel_was_seen = 0;
    total_number_of_frames = 0;
}

std::vector<double> RealEnvironment::fitnessFunction(const Individual::Ptr &ind){
    int env_type = are::settings::getParameter<are::settings::Integer>(parameters,"#envType").value;

    if(env_type == 0)
        return fit_targeted_locomotion();
    else if(env_type == 1)
        return fit_exploration();
    else if(env_type == 2)
        return fit_exploration2();
    else if(env_type == 3)
        return fit_foraging();

    std::cerr << "Unknown type of environment type : " << env_type << std::endl;
    return {0};

}

void RealEnvironment::print_info(){
    // display some infomation about how well the tracking worked
    std::cout<<"Saw the robot in " << number_of_frames_where_robot_was_seen << " frames (out of " << total_number_of_frames<<")"<<std::endl;
    std::cout<<"Saw the barrel in " << number_of_frames_where_barrel_was_seen  << " frames (out of "  << total_number_of_frames <<")"<<std::endl;
    std::cout<<"Fitness computed as:\t";
    Individual::Ptr ind;
    auto fitness = fitnessFunction(ind); // dummy individual in order to call the fitness function
    for(const double val : fitness)
       std::cout << val << "\t";
    std::cout<<std::endl;
}

void RealEnvironment::start_recording(){
    // tell camera to start recording
    std::cout<<"Starting tracking recording"<<std::endl;
    std::string reply_string;
    phy::send_string(reply_string,"start",zmq_tracking_camera_requester_socket,"Recording:");
    assert(reply_string=="OK");
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
std::vector<double> RealEnvironment::fit_exploration2(){
    auto max_fitness = [](int max,std::pair<int,int> init)->int{
        int sum = 0;
        for(int i = 0; i < max; i++)
            for(int j = 0; j < max; j++)
                sum += floor(sqrt((i - init.first)*(i - init.first) + (j - init.second)*(j - init.second)));
        return sum;
    };
    return {static_cast<double>(grid_zone.sum())/static_cast<double>(max_fitness(8,real_coordinate_to_matrix_index(initial_position)))};
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

void RealEnvironment::save_tracking_video(std::string filename)
{
    // tell camera to stop recording
    std::cout<<"Telling tracking to save file as "<<filename<<std::endl;

    std::string reply_string;
    phy::send_string(reply_string,"save_"+filename,zmq_tracking_camera_requester_socket,"Recording:");
    assert(reply_string=="OK");
}

void RealEnvironment::discard_tracking_video()
{
    // tell camera to stop recording but discard the video
    std::string reply_string;
    phy::send_string(reply_string,"discard",zmq_tracking_camera_requester_socket,"Recording:");
    assert(reply_string=="OK");
}

void RealEnvironment::update_info(double time){

    bool robot_seen=false;
    bool barrel_seen=false;

    if(time == 0)
        trajectory.clear();

    // update position of robot
    std::string robot_position_string;
    phy::send_string(robot_position_string,"position",zmq_tracking_camera_requester_socket,"Robot:");

    if(robot_position_string != "None"){ // tracking found a robot position, else the robot wasn't seen, so we will not update the current_position variable
        robot_seen=true;
        // parse the string, expected format is [x, y]
        boost::erase_all(robot_position_string, "[");
        boost::erase_all(robot_position_string, "]");
        std::vector<std::string> robot_position_split_string;
        boost::split(robot_position_split_string, robot_position_string, boost::is_any_of(","));

        current_position = { std::stod(robot_position_split_string[0]) , std::stod(robot_position_split_string[1]) };
    }


    // update position of barrel
    std::string aruco_tags_string;
    phy::send_string(aruco_tags_string,"position",zmq_tracking_camera_requester_socket,"Tags:");
    if (aruco_tags_string!="[]"){
        int tag_number_of_barrel = settings::getParameter<settings::Integer>(parameters,"#arucoTagOnBarrel").value;
        // see if we can find the barrel amoung the list of seen aruco tags
        // expect a string of this format:
        // [[tag_number, x, y], [tag_number, x, y] ... [tag_number, x, y]]
        boost::erase_all(aruco_tags_string, ","); // remove the original commas - we'll just use the brackets and spaces to delimit
        boost::replace_all(aruco_tags_string, "] [",","); // put commas between each tag
        boost::erase_all(aruco_tags_string, "["); // remove any other brackets
        boost::erase_all(aruco_tags_string, "]");

        std::vector<std::string> aruco_tags_split_string;
        boost::split(aruco_tags_split_string, aruco_tags_string, boost::is_any_of(","));
        //each string in aruco_tags_split_string is now an individual tag, spaces between the numbers, e.g. "10 0.1 1.2" would be tag 10 at (0.1,1.2)

        for (int i = 0; i < aruco_tags_split_string.size(); i++){
            std::string this_tag_string = aruco_tags_split_string[i];
            std::vector<std::string> this_tag_string_split;
            boost::split(this_tag_string_split, this_tag_string, boost::is_any_of(" "));
            // tag ID number is this_tag_string_split[0]
            if ( std::stoi(this_tag_string_split[0]) == tag_number_of_barrel){
                // this is the correct tag, so save the location
                barrel_seen=true;
                beacon_position = { std::stod( this_tag_string_split[1] ),std::stod( this_tag_string_split[2])};
            }
        }
    }
    int env_type = are::settings::getParameter<are::settings::Integer>(parameters,"#envType").value;

    // (for exploration task) compute which grid zone the robot is in:
    std::pair<int,int> indexes = real_coordinate_to_matrix_index(current_position);
    if(env_type == 2){
        auto L2 = [](std::pair<int,int> p1, std::pair<int,int> p2)-> int {
            return floor(sqrt((p1.first - p2.first)*(p1.first - p2.first) + (p1.second - p2.second)*(p1.second - p2.second)));
        };
        std::pair<int,int> init_indexes = real_coordinate_to_matrix_index(initial_position);
        std::pair<int,int> indexes = real_coordinate_to_matrix_index(current_position);
        grid_zone(indexes.first,indexes.second) = L2(init_indexes,indexes);
    }
    else grid_zone(indexes.first,indexes.second) = 1;

    // adding waypoint to the trajectory
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

    // keep count of the number of frames:
    total_number_of_frames++;
    if (robot_seen) number_of_frames_where_robot_was_seen++;
    if (barrel_seen) number_of_frames_where_barrel_was_seen++;

    final_position = current_position;

}

std::pair<int,int> RealEnvironment::real_coordinate_to_matrix_index(const std::vector<double> &pos){
    std::pair<int,int> indexes;

    indexes.first = std::trunc(pos[0]/0.25 + 4);
    indexes.second = std::trunc(pos[1]/0.25 + 4);
    // limit each index to the range 0 to grid_zone_size-1:
    if(indexes.first<0) indexes.first=0;
    if(indexes.first>=grid_zone_size) indexes.first=grid_zone_size-1;
    if(indexes.second<0) indexes.second=0;
    if(indexes.second>=grid_zone_size) indexes.second=grid_zone_size-1;

//    if(indexes.first == 8)
//        indexes.first = 7;
//    if(indexes.second == 8)
//        indexes.second = 7;
    return indexes;
}

