#include "simulator.hpp"

using namespace are::sim;
using namespace jsoncons;

bool Simulator::connect(const std::string &address, int port, int timeout){
    _client = std::make_shared<RemoteAPIClient>("localhost",port);
    _sim = std::make_shared<RemoteAPIObject::sim>(_client.get());
    _address = address;
    _port = port;
    _timeout = timeout;
    return true;
}

bool Simulator::reconnect(){
    _client = std::make_shared<RemoteAPIClient>("localhost",_port);
    _sim = std::make_shared<RemoteAPIObject::sim>(_client.get());
    return true;
}


std::vector<std::string> Simulator::call_function(const std::string &fct, const std::vector<std::string> &arguments){
    json args(json_array_arg);
    for(const std::string &arg: arguments)
        args.push_back(arg);
    json ret = _sim->callScriptFunction(fct,_sim->getScript(sim_scripttype_addonscript,"ARE"),args);
    std::vector<std::string> returns;
    for(size_t i = 0; i< ret.size(); i++)
        returns.push_back(ret[i].as<std::string>());
    return returns;
}

bool Simulator::start(bool stepping){
    _sim->setStepping(stepping);
    _sim->startSimulation();
    return (state() == _sim->simulation_advancing_firstafterstop);
}

bool Simulator::step(){
    _sim->step();
    return (state() == _sim->simulation_advancing_running);
}

bool Simulator::stop(){
    _sim->stopSimulation(true);
    return (state() == _sim->simulation_stopped);
}

bool Simulator::run(double duration){
    if(!start())
    return false;
    
    while(duration > time())
        if(state() != _sim->simulation_advancing_running)
            return false;
    _individual_ready = true;
    return stop();
}

bool Simulator::run_stepping(double duration){
    if(!start(true))
    return false;
    while(duration > time())
        if(!step())
            return false;
    _individual_ready = true;
    return stop();
}

int Simulator::state(){
    return _sim->getSimulationState();
}

double Simulator::time(){
    return _sim->getSimulationTime();
}

std::string Simulator::state_to_string(int state){
    if(state == sim_simulation_stopped)
        return "simulation stopped";
    else if(state == sim_simulation_advancing_abouttostop)
        return "simulation about to stop";
    else if(state == sim_simulation_advancing_firstafterstop)
        return "simulation first after stop";
    else if(state == sim_simulation_advancing_firstafterpause)
        return "simulation first after pause";
    else if(state == sim_simulation_advancing_lastbeforepause)
        return "simulation last before pause";
    else if(state == sim_simulation_advancing_lastbeforestop)
        return "simulation last before stop";
    else if(state == sim_simulation_advancing_running)
        return "simulation running";
    else if(state == sim_simulation_paused)
        return "simulation paused";
    else
        return "unknown state";

}
