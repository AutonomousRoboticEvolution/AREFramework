#ifndef PI_COMMUNICATION_HPP
#define PI_COMMUNICATION_HPP

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

namespace are {

namespace phy {


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
void send_string(std::string& str_rpl, const std::string& str_msg, zmq::socket_t& request);

/**
 * @brief receive a string message and send a reply
 * @param message
 * @param reply
 * @param communication socket of reply type (ZMQ_REP)
 */
void receive_string(std::string& str_msg, const std::string& str_req, zmq::socket_t& reply);

void send_string_no_reply(const std::string &str_mess, zmq::socket_t &socket);
void receive_string_no_reply(std::string &str_mess, zmq::socket_t &socket);


bool send_order(OrderType order, zmq::socket_t& publisher);
void receive_string_no_reply(std::string& str_mess, zmq::socket_t& subscriber);

}//phy

}//are

#endif //PI_COMMUNICATION_HPP
