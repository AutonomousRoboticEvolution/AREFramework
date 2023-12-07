/**
	@file v_repExtER.cpp
    @authors Edgar Buchanan, Wei Li, Matteo de Carlo and Frank Veenstra
	@brief This files from the three arguments decides whether to start simulation in local or server mode,
    specifies seed and repository.
*/

#pragma once
#ifndef SIMER_H
#define SIMER_H

#define LIN_SIM

#include <ctime>
#include <memory>
#include "simulatedER/ER.h"
#include "ARE/exp_plugin_loader.hpp"
#include "ERClient/sim_state.h"
#include <simLib/simTypes.h>
#include <simLib/simExp.h>

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

// The 3 required entry points of the plugin:
/// Initialize CoppeliaSim plugin and starts evolution.
SIM_DLLEXPORT int simInit(SSimInit*);
/// Release the CoppeliaSim lib
SIM_DLLEXPORT void simCleanup();
/// Handle different message of simulation
SIM_DLLEXPORT void simMsg(SSimMsg*);

void localMessageHandler(int message);
void clientMessageHandler(int message);

SIM_DLLEXPORT void simInit_ui(); // called immediately after simInit
SIM_DLLEXPORT void simMsg_ui(SSimMsg_ui* info);
SIM_DLLEXPORT void simCleanup_ui(); // called immediately before simCleanup

#endif //SIMEXTER_H

