#define MESSAGE_DEBUG_PRINTING false

#include "physicalER/pi_communication.hpp"
//using namespace are::phy;

void are::phy::send_string(std::string& str_rpl, const std::string& str_msg,  zmq::socket_t& socket,const std::string &topic){
    // send message:
    send_string_no_reply(str_msg,socket,topic);

    // get reply:
    receive_string_no_reply(str_rpl, socket,topic);
}

void are::phy::receive_string(std::string& str_msg, const std::string& str_reply, zmq::socket_t& socket,const std::string &topic){
    // receive a message
    receive_string_no_reply(str_msg, socket,topic);

    //send the reply
    send_string_no_reply(str_reply, socket,topic);
}

void are::phy::send_string_no_reply(const std::string &message_string, zmq::socket_t &socket,const std::string& topic){
    std::string string_to_send;

    if (message_string.substr(0,topic.size())==topic){
        string_to_send=message_string;
    }else{// add topic prefix
        string_to_send=topic+message_string;
    }

    if(MESSAGE_DEBUG_PRINTING) std::cout<<"Sending \""<<string_to_send<<"\" which is length "<<string_to_send.size()<<std::endl;
    socket.send( zmq::buffer( string_to_send ));
}

void are::phy::receive_string_no_reply(std::string &message_string, zmq::socket_t &socket, const std::string &topic){
    zmq::message_t message;
    socket.recv(&message);
    message_string = static_cast<char*>(message.data());
    message_string.resize(message.size()); // truncate to the right length, otherwise static_cast reads extra (nonsense) data
    if(MESSAGE_DEBUG_PRINTING) std::cout<<"Received string \""<<message_string<<"\" which is length "<<message_string.size()<<std::endl;

    // remove topic prefix:
    if (message_string.substr(0,topic.size())==topic){
        message_string=message_string.substr(topic.size(),message_string.size());
    }

}
