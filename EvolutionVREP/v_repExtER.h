/**
	@file v_repExtER.cpp
    @authors Edgar Buchanan, Wei Li, Matteo de Carlo and Frank Veenstra
	@brief This files from the three arguments decides whether to start simulation in local or server mode,
    specifies seed and repository.
*/

#pragma once

#include <ctime>
#include <memory>
#include "ERFiles/ER_VREP.h"


/// an unique pointer to ER_VREP class
std::unique_ptr<ER_VREP> ER;
// Used to indicate the initial call
// It shouldn't matter, maybe it didn;t update
bool initCall = true; // hello
// TODO: EB  do we need these variables? I guess they are used for client-server mode.
int counter = 0;
int timeCount = 0;
bool timerOn = false;
double timeElapsed;
bool loadingPossible = true; // Indicate whether the plugin is ready to accept/load genome sent from client
clock_t sysTime; // Measure simulation time.

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