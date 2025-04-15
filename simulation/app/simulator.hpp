#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <optional>
#include <simLib/simLib.h>
#include <jsoncons/json.hpp>
// #include "RemoteAPIObjects.h"
#include "RemoteAPIClient.h"
#include "simulatedER/zmq_com.hpp"

namespace are {

namespace sim{

class Simulator{
public:
    typedef std::unique_ptr<Simulator> Ptr;
    typedef std::unique_ptr<const Simulator> ConstPtr;

    Simulator(){}
    Simulator(const Simulator& sim) :
        _sim(sim._sim),
        _client(sim._client),
        _timeout(sim._timeout),
        _port(sim._port),
        _address(sim._address),
        _individual_ready(sim._individual_ready)
    {}

    
    bool connect(const std::string &address, int port, int timeout);
    bool reconnect();

    std::vector<std::string> call_function(const std::string &fct, const std::vector<std::string> &arguments);

    /**
     * @brief start the simulation in stepping mode
     * @param wether to activate stepping mode or not
     * @return true if the simulation started successfully, false otherwise
     */
    bool start(bool stepping = false);

    /**
     * @brief step the simulation
     * @return true if the simulation is running, false otherwise
     */
    bool step();

    /**
     * @brief stop the simulation
     * @return true if the simulation has stopped, false otherwise
     */
    bool stop();

    /**
     * @brief run the simulation for a set duration in seconds
     * @param duration of the simulation in seconds
     * @return true if the simulation stopped normally, false otherwise
     */
    bool run(double duration);

    /**
     * @brief run the simulation in stepping mode for a set duration in seconds
     * @param duration of the simulation in seconds
     * @return true if the simulation stopped normally, false otherwise
     */
    bool run_stepping(double duration);

    /**
     * @brief querry the current state of the simulator
     * @return the current state of the simulator
     */
    int state();

    /**
     * @brief querry the current simulation time
     * @return the current simulation time;
     */
    double time();



    const  std::shared_ptr<RemoteAPIObject::sim> &get(){return _sim;}

    bool is_individual_ready(){return _individual_ready;}
    void ready_for_new_sim(){_individual_ready = false;}
    void sim_started(){_individual_ready = true;}

    int port(){return _port;}
    const std::string &address(){return _address;}

    static std::string state_to_string(int state);

private:
    std::shared_ptr<RemoteAPIObject::sim> _sim;
    std::shared_ptr<RemoteAPIClient> _client;
    // RemoteAPIObject::sim _sim;
    int _timeout = 100000;
    int _port = 10000;
    std::string _address;
    bool _individual_ready = false;

};//Simulator
}//sim
}//are

#endif //SIMULATOR_HPP
