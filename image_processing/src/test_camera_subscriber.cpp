#include <iostream>
#include <unistd.h>
#include <zmq.hpp>
#include "physicalER/pi_communication.hpp"

#define MESSAGE_DEBUG_PRINTING false

int main(int argc, char** argv){

    std::cout<<"Starting test_camera_subscriber"<<std::endl;

    if(argc != 3){
        std::cout << "usage: address of publisher, sleep time (micro seconds)" << std::endl;
        return 1;
    }
    zmq::context_t context;
    zmq::socket_t zmq_requester_socket(context, ZMQ_REQ);
    zmq_requester_socket.connect(argv[1]);
    int delay_time = std::stoi(argv[2]);
    std::string robot_position=" ",tags_position=" ";
    while(1){
        // request robot position, by sending string "position" to the topic "Robot:",
        // and save the reply to string called robot_position
        std::string new_robot_position;
        are::phy::send_string(new_robot_position,"position",zmq_requester_socket,"Robot:");
        if(robot_position != new_robot_position){
            std::cout << "Robot position string : " << robot_position << std::endl;
            robot_position = new_robot_position;
        }


        std::string new_tags_position;
        are::phy::send_string(new_tags_position,"position",zmq_requester_socket,"Tags:");
        if(tags_position != new_tags_position){
            std::cout << "Tags position string : " << tags_position << std::endl;
            tags_position = new_tags_position;
        }

        usleep(delay_time);
    }


    return 1;
}
