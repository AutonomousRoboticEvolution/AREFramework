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

void send_ctrl_genome(std::string& str_rpl, const std::string& genome, zmq::socket_t& request);

bool send_order(OrderType order, zmq::socket_t& publisher);
void wait_for_message(std::string& str_mess, zmq::socket_t& subscriber);

}//phy

}//are

#endif //PI_COMMUNICATION_HPP
