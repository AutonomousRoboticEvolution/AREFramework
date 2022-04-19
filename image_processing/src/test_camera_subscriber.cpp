#include <iostream>
#include <unistd.h>
#include <zmq.hpp>

#define MESSAGE_DEBUG_PRINTING false

void send_string_no_reply(const std::string &message_string, zmq::socket_t &socket,const std::string& topic){
    std::string string_to_send;

    if (message_string.substr(0,topic.size())==topic){
        string_to_send=message_string;
    }else{// add "pi " prefix
        string_to_send=topic+message_string;
    }

    if(MESSAGE_DEBUG_PRINTING) std::cout<<"Sending \""<<string_to_send<<"\" which is length "<<string_to_send.size()<<std::endl;
    socket.send( zmq::buffer( string_to_send ));
}

void receive_string_no_reply(std::string &message_string, zmq::socket_t &socket, const std::string &topic){
    zmq::message_t message;
    socket.recv(&message);
    message_string = static_cast<char*>(message.data());
    message_string.resize(message.size()); // truncate to the right length, otherwise static_cast reads extra (nonsense) data
    if(MESSAGE_DEBUG_PRINTING) std::cout<<"Received string \""<<message_string<<"\" which is length "<<message_string.size()<<std::endl;

    // remove "pi " prefix:
    if (message_string.substr(0,topic.size())==topic){
        message_string=message_string.substr(topic.size(),message_string.size());
    }

}

void send_string(std::string& str_rpl, const std::string& str_msg,  zmq::socket_t& socket,const std::string &topic){
    // send message:
    send_string_no_reply(str_msg,socket,topic);

    // get reply:
    receive_string_no_reply(str_rpl, socket,topic);
}

void receive_string(std::string& str_msg, const std::string& str_reply, zmq::socket_t& socket,const std::string &topic){
    // receive a message
    receive_string_no_reply(str_msg, socket,topic);

    //send the reply
    send_string_no_reply(str_reply, socket,topic);
}




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
        send_string(robot_position,"position",zmq_requester_socket,"Robot:");
        std::cout << "Robot position string : " << robot_position << std::endl;


        std::string tags_position;
        send_string(tags_position,"position",zmq_requester_socket,"Tags:");
        std::cout << "Tags position string : " << tags_position << std::endl;

        usleep(delay_time);
    }


    return 1;
}
