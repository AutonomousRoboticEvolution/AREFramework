#include "RemoteAPIClient.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <random>
#include <sstream>

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}

json bin(const char *s, int size)
{
    if(size == -1) size = int(strlen(s));
    return bin(reinterpret_cast<const uint8_t *>(s), size);
}

json bin(const uint8_t *b, int size)
{
    return bin(std::vector<uint8_t>{b, b + size});
}

json bin(const std::string &s)
{
    return bin(s.data(), int(s.length()));
}

json bin(const std::vector<uint8_t> &v)
{
    return json{byte_string_arg, v};
}

RemoteAPIClient::RemoteAPIClient(const std::string host_, int rpcPort, int cntPort, int verbose_)
    :verbose(verbose_), host(host_), rpc_port(rpcPort), cnt_port(cntPort)
{    
    if(verbose == -1)
    {
        if(const char* verboseStr = std::getenv("VERBOSE"))
            verbose = std::atoi(verboseStr);
        else
            verbose = 0;
    }

    uuid = uuid::generate_uuid_v4();
    VERSION = 2;
    connect();
}


RemoteAPIClient::~RemoteAPIClient()
{
    json req;
    req["func"] = "_*end*_";
    req["args"] = json::array();
    send(req);
    recv();
}

void RemoteAPIClient::connect(){
    rpcSocket = zmq::socket_t(ctx,zmq::socket_type::req);
    auto rpcAddr = (boost::format("tcp://%s:%d") % host % rpc_port).str();
    rpcSocket.connect(rpcAddr);
    rpcSocket.set(zmq::sockopt::linger,0);
}

json RemoteAPIClient::call(const std::string &func, std::initializer_list<json> args, int timeout)
{
    return call(func, json::make_array(args),timeout);
}

json RemoteAPIClient::call(const std::string &_func, const json &_args, int timeout)
{ // call function with specified arguments. Is reentrant
    std::string func(_func);
    json args = _args;
    json req;
    req["func"] = func;
    req["args"] = args;
    send(req);

    json resp = recv(timeout);

    while (resp.contains("func"))
    { // We have a callback or a wait:
        if (resp["func"].as<std::string>().compare("_*wait*_")==0)
        {
            func = "_*executed*_";
            args = json::array();
            json req2;
            req2["func"] = func;
            req2["args"] = args;
            send(req2);
        }
        else if (resp["func"].as<std::string>().compare("_*repeat*_")==0)
        {
            json req2;
            req2["func"] = func;
            req2["args"] = args;
            send(req2);
        }
        else
        { // call a callback
            auto funcToRun = _getFunctionPointerByName(resp["func"].as<std::string>());
            json rep;
            func = "_*executed*_";
            if (funcToRun)
            {
                auto r = funcToRun(resp["args"]);
                if (!r.is_array())
                {
                    auto arr = json::array();
                    arr.push_back(r);
                    args = arr;
                }
                else
                    args = r;
            }
            else
                args = json::array();
            rep["func"] = func;
            rep["args"] = args;
            send(rep);
        }
        resp = recv(timeout);
    }

    if (resp.contains("err"))
        throw std::runtime_error(resp["err"].as<std::string>().c_str());
    const auto &ret = resp["ret"];
    return ret;
}

json RemoteAPIClient::getObject(const std::string &name)
{
    return call("zmqRemoteApi.info", json(json_array_arg, {name}));
}

void RemoteAPIClient::require(const std::string &name)
{
    call("zmqRemoteApi.require", json(json_array_arg, {name}));
}

void RemoteAPIClient::setVerbose(int level)
{
    verbose = level;
}

void RemoteAPIClient::setStepping(bool enable)
{ // for backw. comp., now via sim.setStepping
    call("sim.setStepping", {enable});
}

void RemoteAPIClient::step(bool wait)
{ // for backw. comp., now via sim.step
    call("sim.step", {wait});
}

void RemoteAPIClient::send(json &j)
{
    if(verbose > 0)
        std::cout << "Sending: " << pretty_print(j) << std::endl;

    j["uuid"] = uuid;
    j["ver"] = VERSION;
    j["lang"] = "c++";
    if (j.contains("args"))
    {
        auto a = j["args"];
        j["argsL"] = a.size();
    }


    std::vector<uint8_t> data;
    cbor::encode_cbor(j, data);

    if(verbose > 1)
    {
        std::cout << "Sending (raw):";
        for(size_t i = 0; i < data.size(); i++)
            std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << int(data[i]);
        std::cout << std::endl;
    }

    zmq::message_t msg(data.data(), data.size());
    rpcSocket.send(msg, zmq::send_flags::dontwait);
}

json RemoteAPIClient::recv(int timeout)
{
    zmq::pollitem_t items[] = {{rpcSocket,0,ZMQ_POLLIN,0}};
    zmq::poll(&items[0],1,timeout);
    if(items[0].revents & ZMQ_POLLIN){
        zmq::message_t msg;
        rpcSocket.recv(msg);
        auto data = reinterpret_cast<const uint8_t*>(msg.data());

        if(verbose > 1)
        {
            std::cout << "Received (raw):";
            for(size_t i = 0; i < msg.size(); i++)
                std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << int(data[i]);
            std::cout << std::endl;
        }

        json j = cbor::decode_cbor<json>(data, data + msg.size());

        if(verbose > 0)
            std::cout << "Received: " << pretty_print(j) << std::endl;

        return j;

    }else{
        std::cerr << "Did not receive reply" << std::endl;
        connect();
        json j;
        j["ret"] = -1;
        j["err"] = "no reply";
        return j;
    }

}

void RemoteAPIClient::registerCallback(const std::string &funcName, CallbackType callback)
{
    callbacks[funcName] = callback;
}

RemoteAPIClient::CallbackType RemoteAPIClient::_getFunctionPointerByName(const std::string &funcName)
{
    auto it = callbacks.find(funcName);
    if(it != callbacks.end())
        return it->second;
    return nullptr;
}

#ifdef SIM_REMOTEAPICLIENT_OBJECTS
#include "RemoteAPIObjects.cpp"
#endif // SIM_REMOTEAPICLIENT_OBJECTS
