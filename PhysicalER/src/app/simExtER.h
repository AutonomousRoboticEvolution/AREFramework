#pragma once
#ifndef SIMEXTER_H
#define SIMEXTER_H

#include <ctime>
#include <memory>
#include "/home/ebb505/ARE_Project/evolutionary_robotics_framework/EAFramework/include/ARE/exp_plugin_loader.hpp"

/// an unique pointer to ER_VREP class
//are::sim::ER::Ptr ERVREP;

/// This variable marks the start of evolution.
bool startEvolution;

#ifdef _WIN32
    #define SIM_DLLEXPORT extern "C" __declspec(dllexport)
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
    #define SIM_DLLEXPORT extern "C"
#endif /* __linux || __APPLE__ */

// The 3 required entry points of the plugin:

/// Initialize v-rep plugin and starts evolution.
SIM_DLLEXPORT unsigned char simStart(void* reservedPointer,int reservedInt);
/// Release the v-rep lib
SIM_DLLEXPORT void simEnd();
/// Handle different message of simulation
SIM_DLLEXPORT void* simMessage(int message, int* auxiliaryData, void* customData, int* replyData);

void localMessageHandler(int message);
void clientMessageHandler(int message);

#endif //SIMEXTER_H

