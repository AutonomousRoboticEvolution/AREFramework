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

    while(1){
        // request robot position, by sending string "position" to the topic "Robot:",
        // and save the reply to string called robot_position
        std::string robot_position;
        are::phy::send_string(robot_position,"position",zmq_requester_socket,"Robot:");
        std::cout << "Robot position string : " << robot_position << std::endl;


        std::string tags_position;
        are::phy::send_string(tags_position,"position",zmq_requester_socket,"Tags:");
        std::cout << "Tags position string : " << tags_position << std::endl;

        usleep(delay_time);
    }


    return 1;
}
