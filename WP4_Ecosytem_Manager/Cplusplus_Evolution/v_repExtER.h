// Copyright 2006-2015 Coppelia Robotics GmbH. All rights reserved.
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
//
// -------------------------------------------------------------------
// THIS FILE IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
//
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------
//
// This file was automatically created for V-REP release V3.2.1 on May 3rd 2015

/**
	@file v_repExtER.h
	@brief Class declaration of the plugin
*/

#pragma once

#include <ctime>
#include <memory>
#include "ERFiles/ER_VREP.h"

bool loadingPossible = true;
using namespace std;
unique_ptr<ER_VREP> ER;
bool initCall = true;
// TODO: Check with Frank
/// Variable used but the value is always 0
int counter = 0;
// TODO: Check with Frank
/// Variable not used!
bool initialized = false;
int timeCount = 0;
/// This variable marks the start of evolution.
bool startEvolution = true;
/// Measure simulation time.
clock_t sysTime;

#ifdef _WIN32
	#define VREP_DLLEXPORT extern "C" __declspec(dllexport)
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	#define VREP_DLLEXPORT extern "C"
#endif /* __linux || __APPLE__ */

// The 3 required entry points of the plugin:
/**
	@brief Initialize v-rep plugin and starts evolution.
	@return unsigned char; v-rep version if operation successful, false if not
*/
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt);
/**
	@brief Release the v-rep lib
*/
VREP_DLLEXPORT void v_repEnd();
/**
	@brief Handle different message of simulation
*/
VREP_DLLEXPORT void* v_repMessage(int message, int* auxiliaryData, void* customData, int* replyData);
