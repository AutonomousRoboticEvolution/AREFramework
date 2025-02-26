#ifndef ZMQ_COM_HPP
#define ZMQ_COM_HPP

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

namespace are {



typedef enum OrderType{
    LAUNCH = 0,
    ABORT = 1//not in use
} OrderType;


/**
 * @brief send a string message and receive the reply
 * @param reply
 * @param message
 * @param communication socket of request type (ZMQ_REQ)
 */
void send_string(std::string& str_rpl, const std::string& str_msg, zmq::socket_t& request, const std::string &);

/**
 * @brief receive a string message and send a reply
 * @param message
 * @param reply
 * @param communication socket of reply type (ZMQ_REP)
 */
void receive_string(std::string& str_msg, const std::string& str_req, zmq::socket_t& reply, const std::string &);

bool send_string_no_reply(const std::string &str_mess, zmq::socket_t &socket,const std::string& topic);
bool receive_string_no_reply(std::string &str_mess, zmq::socket_t &socket,const std::string &topic);

}//are

#endif //ZMQ_COM_HPP
