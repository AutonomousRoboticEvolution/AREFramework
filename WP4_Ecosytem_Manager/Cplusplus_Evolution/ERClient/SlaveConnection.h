#pragma once

#include <string>
#include <sstream>
#include <exception>

extern "C" {
    // #include "v_repLib.h"
    #include "v_repConst.h"
    #include "remoteApi/extApi.h"
}

#define CONNECTION_TIMEOUT 5000

class SlaveConnection {
public:
    enum State {
        NOT_CONNECTED = -1,
        FREE = 0,
        EVALUATING = 1,
        // RELOAD_GENOME = 9,
        DELAY = 10,
    };

private:
    std::string _address;
    int _port;
    simxInt _clientID;
    int _individual;
    int _individualNum;
    State _state;

public:
	explicit SlaveConnection(const std::string& address, int port);
    SlaveConnection(const SlaveConnection &other);
    SlaveConnection(SlaveConnection &&other);
    virtual ~SlaveConnection();
	int timeOutReconnectAttempt = 0;

    bool connect(int connectionTimeoutMs = CONNECTION_TIMEOUT);
    bool connect(const std::string& address, int port, int connectionTimeoutMs = CONNECTION_TIMEOUT);
    bool status() const;
    void disconnect();
    bool reconnect();
    simxInt getIntegerSignal(const std::string& signalName) const;
    /**
     * Starts the buffer reading of `signalName`
     * @param msinterval [0-65535] - interval of ms when to execute the command and save to buffer.
     */
    simxInt getIntegerSignalStreaming(const std::string& signalName, unsigned short msInterval = 0) const;
    /**
     * @returns -1 if there is no value to read in the buffer
     */
    simxInt getIntegerSignalBuffer(const std::string& signalName) const;
    void setIntegerSignal(const std::string& signalName, simxInt state);
    simxFloat getFloatSignal(const std::string& signalName) const;
    void setFloatSignal(const std::string& signalName, simxFloat state);
    // const std::string getStringSignal(const std::string& signalName) const;
    void setStringSignal(const std::string& signalName, const std::string &state);

    bool operator!() const;

    const std::string& address() const { return _address; } 
    simxInt port() const { return _port; }
    int individual() const { return _individual; }
    int individualNum() const { return _individualNum; }
    State state() const { return _state; }

    void setIndividual(int individual) { _individual = individual; }
    void setIndividualNum(int individualNum) { _individualNum = individualNum; }
    void setState(State state) { _state = state; }
};

class VrepRemoteException: public std::exception
{
public:
    simxInt returnValue;
    std::string message;

    explicit VrepRemoteException(simxInt returnValue)
        : returnValue(returnValue)
    {
        std::stringstream message("An error occoured when using the VREP remote api.");
        message << std::endl;

        if (returnValue & simx_return_novalue_flag) 
            message << "There is no command reply in the input buffer. This should not always be considered as an error, depending on the selected operation mode" << std::endl;
        if (returnValue & simx_return_timeout_flag)
            message << "The function timed out (probably the network is down or too slow)" << std::endl;
        if (returnValue & simx_return_illegal_opmode_flag)
            message << "The specified operation mode is not supported for the given function" << std::endl;
        if (returnValue & simx_return_remote_error_flag)
            message << "The function caused an error on the server side (e.g. an invalid handle was specified)" << std::endl;
        if (returnValue & simx_return_split_progress_flag)
            message << "The communication thread is still processing previous split command of the same type" << std::endl;
        if (returnValue & simx_return_local_error_flag)
            message << "The function caused an error on the client side" << std::endl;
        if (returnValue & simx_return_initialize_error_flag)
            message << "simxStart was not yet called" << std::endl;

        this->message = message.str();
    }

    virtual const char* what() const throw ()
    {
        return this->message.c_str();
    }
};


#undef CONNECTION_TIMEOUT
