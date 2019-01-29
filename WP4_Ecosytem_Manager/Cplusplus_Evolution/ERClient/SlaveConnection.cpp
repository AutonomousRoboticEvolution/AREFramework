#include "SlaveConnection.h"
#include <iostream>

extern "C" {
    #include "v_repLib.h"
    #include "v_repConst.h"
    #include "remoteApi/extApi.h"
}

void checkReturnValue(simxInt returnValue)
{
	if (returnValue == simx_return_ok) {
		return;
	}
	else if (returnValue == simx_return_timeout_flag) {
		// Time-out happens, try again (Something is going on in the server)
		extApi_sleepMs(100);
		return;
	}
	else if (returnValue == simx_return_novalue_flag) {
		// This is fine. 
		extApi_sleepMs(20);
		return; 
	}
	else if (returnValue <= 8) {
		extApi_sleepMs(100);
	}
	throw VrepRemoteException(returnValue);
}

SlaveConnection::SlaveConnection(const std::string& address, int port)
    : _address(address)
    , _port(port)
    , _clientID(-1)
    , _individual(-1)
    , _state(State::NOT_CONNECTED)
{}

SlaveConnection::SlaveConnection(const SlaveConnection &other)
    : _address(other._address)
    , _port(other._port)
    , _clientID(other._clientID)
    , _individual(other._individual)
    , _state(other._state)
{}

SlaveConnection::SlaveConnection(SlaveConnection &&other)
    : _address(std::move(other._address))
    , _port(other._port)
    , _clientID(other._clientID)
    , _individual(other._individual)
    , _state(other._state)
{}

SlaveConnection::~SlaveConnection()
{}

bool SlaveConnection::connect(int connectionTimeoutMs)
{
    simxInt result = simxStart(
        this->_address.c_str(), // connectionAddress
        this->_port,            // connectionPort
        true,                   // waitUntilConnected
        false,                  // doNotReconnectOnceDisconnected
        connectionTimeoutMs,    // timeOutInMs
        5);                     // commThreadCycleInMs

    if (result == -1) {
        return false;
    } else {
        this->_clientID = result;
        this->setState(FREE);
        return true;
    }
}

bool SlaveConnection::connect(const std::string& address, int port, int connectionTimeoutMs)
{ 
    this->_address = address;
    this->_port = port;
    return this->connect(connectionTimeoutMs);
}

bool SlaveConnection::status() const
{
    return this->_clientID != -1 
        && simxGetConnectionId(this->_clientID) != -1;
}

void SlaveConnection::disconnect()
{
    simxFinish(this->_clientID);
    this->_clientID = -1;
    this->_individual = -1;
    this->_state = State::NOT_CONNECTED;
}

bool SlaveConnection::reconnect()
{
    if (this->_clientID != -1)
    {
        this->disconnect();
    }

    return this->connect();
}

simxInt SlaveConnection::getIntegerSignal(const std::string& signalName) const
{
    simxInt states[1];
    checkReturnValue(
		simxGetIntegerSignal(this->_clientID, (simxChar*) signalName.c_str(), states, simx_opmode_blocking)
	);
    return states[0];
}

simxInt SlaveConnection::getIntegerSignalStreaming(const std::string& signalName, unsigned short msInterval) const
{
    simxInt states[1];
    try {
        checkReturnValue(
            simxGetIntegerSignal(
                this->_clientID,
                (simxChar*) signalName.c_str(),
                states,
                simx_opmode_streaming + msInterval
        )
		);
    } catch (const VrepRemoteException &e) {
        if (e.returnValue & simx_return_novalue_flag) {
            // this is to be expected for streaming mode
        } else {
            // something else happened, throw.
            throw;
        }
    }
    return states[0];
}

simxInt SlaveConnection::getIntegerSignalBuffer(const std::string& signalName) const
{
    simxInt states[1];
	checkReturnValue(
        simxGetIntegerSignal(this->_clientID, (simxChar*) signalName.c_str(), states, simx_opmode_buffer)
    );
	return states[0];
}

void SlaveConnection::setIntegerSignal(const std::string& signalName, simxInt state)
{
    checkReturnValue(
        simxSetIntegerSignal(this->_clientID, (simxChar*) signalName.c_str(), state, simx_opmode_blocking)
    );
}

simxFloat SlaveConnection::getFloatSignal(const std::string& signalName) const
{
    simxFloat states[1];
    checkReturnValue(
        simxGetFloatSignal(this->_clientID, (simxChar*) signalName.c_str(), states, simx_opmode_blocking)
    );
    return states[0];
}

void SlaveConnection::setFloatSignal(const std::string& signalName, simxFloat state)
{
    checkReturnValue(
        simxSetFloatSignal(this->_clientID, (simxChar*) signalName.c_str(), state, simx_opmode_blocking)
    );
}

// const std::string SlaveConnection::getStringSignal(const std::string& signalName) const
// {
//     simxFloat states[1];
//     checkReturnValue(
//         simxGetStringSignal(this->_clientID, (simxChar*) signalName.c_str(), states, simx_opmode_blocking)
//     );
//     return states[0];
// }

void SlaveConnection::setStringSignal(const std::string& signalName, const std::string& state)
{
    checkReturnValue(
        simxSetStringSignal(this->_clientID, (simxChar*) signalName.c_str(), (simxUChar*) state.c_str(), state.size(), simx_opmode_blocking)
    );
}

bool SlaveConnection::operator!() const
{
    return this->status();
}