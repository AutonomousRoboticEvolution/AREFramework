#pragma once
#ifndef V_REPEXTGENERATE_H
#define V_REPEXTGENERATE_H

#include <ctime>
#include <memory>
#include "ARE/exp_plugin_loader.hpp"
#include "physicalER/generate/ER_generate.hpp"

namespace are_set = are::settings;
namespace gen = are::phy::generate;

gen::ER::Ptr ER;
int timeCount = 0;

enum SimulationState {
    INITIALIZING,
    FREE,
    STARTING,
    BUSY,
    CLEANUP,
    RESTART
};

SimulationState simulationState = FREE;

#ifdef _WIN32
    #define SIM_DLLEXPORT extern "C" __declspec(dllexport)
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
    #define SIM_DLLEXPORT extern "C"
#endif /* __linux || __APPLE__ */

// The 3 required entry points of the plugin:

/// Initialize v-rep plugin and starts evolution.
SIM_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt);
/// Release the v-rep lib
SIM_DLLEXPORT void v_repEnd();
/// Handle different message of simulation
SIM_DLLEXPORT void* v_repMessage(int message, int* auxiliaryData, void* customData, int* replyData);

void localMessageHandler(int message);
void clientMessageHandler(int message);

#endif //V_REPEXTGENERATE_H

