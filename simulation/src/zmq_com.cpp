#include "simulatedER/zmq_com.hpp"
// using namespace are;


void are::send_string(std::string& str_rpl, const std::string& str_msg,  zmq::socket_t& socket,const std::string &topic){
    // send message:
    send_string_no_reply(str_msg,socket,topic);

    // get reply:
    receive_string_no_reply(str_rpl, socket,topic);
}

void are::receive_string(std::string& str_msg, const std::string& str_reply, zmq::socket_t& socket,const std::string &topic){
    // receive a message
    receive_string_no_reply(str_msg, socket,topic);

    //send the reply
    send_string_no_reply(str_reply, socket,topic);
}

bool are::send_string_no_reply(const std::string &message_string, zmq::socket_t &socket,const std::string& topic){
    std::string string_to_send;

    if (message_string.substr(0,topic.size())==topic){
        string_to_send=message_string;
    }else{// add topic prefix
        string_to_send=topic+message_string;
    }

    auto ret = socket.send(zmq::buffer(string_to_send));
    if(ret.has_value()){
        return !(ret.value() == EAGAIN);
    }    
    return false;
}

bool are::receive_string_no_reply(std::string &message_string, zmq::socket_t &socket, const std::string &topic){
    zmq::message_t message;
    if(socket.recv(&message)){
        message_string.assign(static_cast<char*>(message.data()),message.size());
        //    message_string.resize(message.size()); // truncate to the right length, otherwise static_cast reads extra (nonsense) data

        // remove topic prefix:
        if (message_string.substr(0,topic.size())==topic){
            message_string=message_string.substr(topic.size(),message_string.size());
        }
        return true;
    }

    std::cerr << "message not received : ";
    if(errno == EAGAIN)
        std::cerr << "EAGAIN" << std::endl;
    else if(errno == ENOTSUP)
        std::cerr << "ENOTSUP" << std::endl;
    else if(errno == EFSM)
        std::cerr << "EFSM" << std::endl;
    else if(errno == ENOTSOCK)
        std::cerr << "ENOTSOCK" << std::endl;
    else if(errno == EINTR)
        std::cerr << "EINTR" << std::endl;
    else
        std::cerr << "unknown errno: " << errno << std::endl;

    return false;
}
