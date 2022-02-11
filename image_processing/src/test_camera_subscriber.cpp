#include <iostream>
#include <unistd.h>
#include <zmq.hpp>

int main(int argc, char** argv){

    if(argc =! 1){
        std::cout << "usage: address of publisher" << std::endl;
        return 1;
    }
    zmq::context_t context;
    zmq::socket_t robot_subs(context,ZMQ_SUB);
    zmq::socket_t tags_subs(context,ZMQ_SUB);

    robot_subs.connect(argv[1]);
    robot_subs.set(zmq::sockopt::subscribe, "Robot:");

    tags_subs.connect(argv[1]);
    tags_subs.set(zmq::sockopt::subscribe, "Tags:");
    zmq::message_t robot_pos,tags_pos;
    while(1){

        robot_subs.recv(&robot_pos);

        std::cout << "Robot position : " << static_cast<char*>(robot_pos.data()) << std::endl;

        tags_subs.recv(&tags_pos);

        std::cout << "Tags position : " << static_cast<char*>(tags_pos.data()) << std::endl;

        sleep(1);
    }


    return 0;
}
