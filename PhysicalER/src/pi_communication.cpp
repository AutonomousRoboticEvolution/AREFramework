#include "physicalER/pi_communication.hpp"

//using namespace are::phy;

void are::phy::send_string(std::string& str_rpl, const std::string& str_msg,  zmq::socket_t& request){
    zmq::message_t message(str_msg.size());
    strcpy(static_cast<char*>(message.data()),str_msg.c_str());
    request.send(message);
    zmq::message_t reply;
    request.recv(&reply);
    str_rpl = static_cast<char*>(reply.data());
}

void are::phy::receive_string(std::string& str_msg, const std::string& str_req, zmq::socket_t& reply){
    zmq::message_t reply_msg;
    strcpy(static_cast<char*>(reply_msg.data()),str_req.c_str());
    zmq::message_t message;
    reply.recv(&message);
    reply.send(reply_msg);
    str_msg = static_cast<char*>(message.data());
}

bool are::phy::send_order(OrderType order, zmq::socket_t& request){

//TODO rewrite this function
    //std::cout << "send order" << std::endl;

    zmq::message_t order_msg(1);
    int ord[1] = {static_cast<int>(order)};
    memcpy(order_msg.data(),ord,1);

    request.send(order_msg);
    zmq::message_t reply;
    request.recv(&reply);


    if(strcmp(static_cast<char*>(reply.data()),"order recieved")!=0){
        std::cerr << "Connection Lost to : Time Out" << std::endl;
        return false;
    }
    else return true;
}

void are::phy::wait_for_message(std::string &str_mess, zmq::socket_t &subscriber){
    //std::cout << "wait for message" << std::endl;

    zmq::message_t msg;
    subscriber.recv(&msg);
    str_mess = static_cast<char*>(msg.data());
}
