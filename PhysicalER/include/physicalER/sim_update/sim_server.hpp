#ifndef SIM_SERVER_HPP
#define SIM_SERVER_HPP

#include <ctime>
#include <memory>
#include "physicalER/update/ER_update.hpp"
#include "ARE/exp_plugin_loader.hpp"
#include "sim_are_control.hpp"
#include "ARE/nn2/NN2CPPNGenome.hpp"
#include "ARE/NNParamGenome.hpp"
#include "simulatedER/VirtualEnvironment.hpp"
#include "physicalER/pi_communication.hpp"

/// an unique pointer to ER_VREP class
are::phy::update::ER::Ptr ERVREP;
zmq::context_t context (1);
zmq::socket_t publisher (context, ZMQ_PUB);
zmq::socket_t reply (context, ZMQ_REP);
are::sim::AREControl are_ctrl;
bool sim_started = false;

#ifdef _WIN32
	#define VREP_DLLEXPORT extern "C" __declspec(dllexport)
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	#define VREP_DLLEXPORT extern "C"
#endif /* __linux || __APPLE__ */

// The 3 required entry points of the plugin:

/// Initialize v-rep plugin and starts evolution.
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt);
/// Release the v-rep lib
VREP_DLLEXPORT void v_repEnd();
/// Handle different message of simulation
VREP_DLLEXPORT void* v_repMessage(int message, int* auxiliaryData, void* customData, int* replyData);

void localMessageHandler(int message);
void clientMessageHandler(int message);

#endif //SIM_SERVER_HPP
