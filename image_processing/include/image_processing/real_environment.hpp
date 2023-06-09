#ifndef REAL_ENVIRONMENT_HPP
#define REAL_ENVIRONMENT_HPP

#include "physicalER/pi_communication.hpp"
#include "ARE/Environment.h"
#include <zmq.hpp>
#include <chrono>
#include <boost/algorithm/string.hpp> // useful for parsing the received messages


namespace are {

class RealEnvironment : public Environment
{
public:
    RealEnvironment();
    void init() override;
    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    void print_info() override;
    void update_info(double time) override;
    const Eigen::MatrixXi &get_visited_zone_matrix(){return grid_zone;}

    std::pair<int,int> real_coordinate_to_matrix_index(const std::vector<double> &pos);

    std::vector<double> fit_targeted_locomotion();
    std::vector<double> fit_exploration();
    std::vector<double> fit_exploration2();
    std::vector<double> fit_foraging();

    void save_tracking_video(std::string filename);
    void discard_tracking_video();
    void start_recording();

    // some infomation about how successful the tracking was
    int total_number_of_frames=0;
    int number_of_frames_where_robot_was_seen=0;
    int number_of_frames_where_barrel_was_seen=0;

private:
    std::vector<double> initial_position;
    std::vector<double> current_position;
    std::vector<double> target_position;
    std::vector<double> beacon_position;
    const int grid_zone_size = 8;
    Eigen::MatrixXi grid_zone;

    bool target_reached = false;

    bool usingIPCamera;


    // zmq stuff for getting messages from python camera program
    zmq::context_t context;
    zmq::socket_t zmq_tracking_camera_requester_socket;
    zmq::socket_t robot_pos_subs;
    zmq::socket_t tags_pos_subs;

    bool robot_ever_seen; // for detecting if we are at the start of a new trajectory, but have never detected the robot

};

} //are

#endif //REAL_ENVIRONMENT_HPP
