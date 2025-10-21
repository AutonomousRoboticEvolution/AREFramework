#include "simulator.hpp"

using namespace are::sim;
using namespace jsoncons;

bool Simulator::connect(const std::string &address, int port, int timeout){
    _address = address;
    _port = port;
    _timeout = timeout;
    _client = std::make_shared<RemoteAPIClient>(address,port);
    try{
        _sim = std::make_shared<RemoteAPIObject::sim>(_client.get());
    }catch(const std::runtime_error& error){
        _print_error(__func__,error);
        return false;
    }


    return true;
}

bool Simulator::reconnect(){
    _client = std::make_shared<RemoteAPIClient>(_address,_port);
    try{
        _sim = std::make_shared<RemoteAPIObject::sim>(_client.get());
    }catch(const std::runtime_error& error){
        _print_error(__func__,error);
        return false;
    }
    return true;
}


std::vector<std::string> Simulator::call_function(const std::string &fct, const std::vector<std::string> &arguments, int timeout){
    json args(json_array_arg);
    for(const std::string &arg: arguments)
        args.push_back(arg);
    json ret;
    try{
        ret = _sim->callScriptFunction(fct,_sim->getScript(sim_scripttype_addonscript,"ARE"),args,timeout);
    }catch(const std::runtime_error& error){
        std::vector<std::string> args = {fct};
        args.insert(args.end(),arguments.begin(),arguments.end());
        std::stringstream sstr;
        sstr << timeout;
        args.push_back(sstr.str());
        _print_error(__func__,error,args);
        return {};
    }

    std::vector<std::string> returns;
    for(size_t i = 0; i< ret.size(); i++)
        returns.push_back(ret[i].as<std::string>());
    return returns;
}

bool Simulator::start(bool stepping){
    try{
        _sim->setStepping(stepping);
        _sim->startSimulation();

        return (state() == _sim->simulation_advancing_firstafterstop);
    }catch(const std::runtime_error& error){
        _print_error(__func__,error);
        return false;
    }
}

bool Simulator::step(){
    try{
        _sim->step();
        return (state() == _sim->simulation_advancing_running);
    }catch(const std::runtime_error& error){
        _print_error(__func__,error);
        return false;
    }
}

bool Simulator::stop(){
    try{
        _sim->stopSimulation(true);
        return (state() == _sim->simulation_stopped);
    }catch(const std::runtime_error& error){
        _print_error(__func__,error);
        return false;
    }
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
    try{
        return _sim->getSimulationState();
    }catch(const std::runtime_error& error){
        _print_error(__func__,error);
        return -1;
    }
}

double Simulator::time(){
    try{
        return _sim->getSimulationTime();
    }catch(const std::runtime_error& error){
        _print_error(__func__,error);
        return -1;
    }
}

void Simulator::_print_error(const std::string &fct_name,const std::runtime_error& error,  const std::vector<std::string> fct_args){
    std::cerr << "Error in " << fct_name << "(";
    for(const std::string &arg: fct_args)
        std::cerr << arg << ",";
    std::cerr << ")" << " of sim with port " << _port << " " << error.what() << std::endl;
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
