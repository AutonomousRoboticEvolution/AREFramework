/**
	@file v_repExtER.cpp
    @authors Edgar Buchanan, Wei Li, Matteo de Carlo and Frank Veenstra
	@brief This files from the three arguments decides whether to start simulation in local or server mode,
    specifies seed and repository.
*/

#ifndef V_REPEXTER_H
#define V_REPEXTER_H

#include <ctime>
#include <memory>
#include "simulatedER/ER.h"
#include "ARE/exp_plugin_loader.hpp"
#include "ERClient/sim_state.h"

/// an unique pointer to ER_VREP class
are::sim::ER::Ptr ERVREP;
// TODO: EB  do we need these variables? I guess they are used for client-server mode.
int counter = 0;
int timeCount = 0;
bool timerOn = false;
double timeElapsed;
bool loadingPossible = true; // Indicate whether the plugin is ready to accept/load genome sent from client
clock_t sysTime; // Measure simulation time.

enum SimulationState {
    INITIALIZING,
    FREE,
    STARTING,
    BUSY,
    CLEANUP,
    RESTART
};

SimulationState simulationState = FREE;

/// This variable marks the start of evolution.
bool startEvolution;

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

#endif //V_REPEXTER_H
