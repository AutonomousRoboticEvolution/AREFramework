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

#include "v_repExtER.h"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include "v_repLib.h"

using namespace std;
//CCatContainer* catContainer = NULL;
//CCreatureCreator* creatureCreator = NULL;
//CUserInterface* userInterfaceCreator = NULL;

#ifdef _WIN32
	#ifdef QT_COMPIL
		#include <direct.h>
	#else
		#include <shlwapi.h>
		#pragma comment(lib, "Shlwapi.lib")
	#endif
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
	#include <unistd.h>
	#define WIN_AFX_MANAGE_STATE
#endif /* __linux || __APPLE__ */

#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)	CONCAT(x,y,z)

LIBRARY vrepLib;

VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer, int reservedInt)
{ // This is called just once, at the start of V-REP.
	// Dynamically load and bind V-REP functions:
	char curDirAndFile[1024];
#ifdef _WIN32
#ifdef QT_COMPIL
	_getcwd(curDirAndFile, sizeof(curDirAndFile));
#else
	GetModuleFileName(NULL, curDirAndFile, 1023);
	PathRemoveFileSpec(curDirAndFile);
#endif
#elif defined (__linux) || defined (__APPLE__)
	getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif

	std::string currentDirAndPath(curDirAndFile);
	std::string temp(currentDirAndPath);

#ifdef _WIN32
	temp += "\\v_rep.dll";
#elif defined (__linux)
	temp += "/libv_rep.so";
#elif defined (__APPLE__)
	temp += "/libv_rep.dylib";
#endif /* __linux || __APPLE__ */

	vrepLib = loadVrepLibrary(temp.c_str());
	if (vrepLib == NULL)
	{
		std::cout << "Error, could not find or correctly load v_rep.dll. Cannot start 'BubbleRob' plugin.\n";
		return(0); // Means error, V-REP will unload this plugin
	}
	if (getVrepProcAddresses(vrepLib) == 0)
	{
		std::cout << "Error, could not find all required functions in v_rep.dll. Cannot start 'BubbleRob' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	// Check the V-REP version:
	int vrepVer;
	simGetIntegerParameter(sim_intparam_program_version, &vrepVer);
	if (vrepVer < 30200) // if V-REP version is smaller than 3.02.00
	{
		std::cout << "Sorry, your V-REP copy is somewhat old, V-REP 3.2.0 or higher is required. Cannot start 'BubbleRob' plugin.\n";
		unloadVrepLibrary(vrepLib);
		return(0); // Means error, V-REP will unload this plugin
	}

	// make sure necessary folders are in place // doesn't work on linux
	//string OutputFolder = "interfaceFiles";
	//if (CreateDirectory(OutputFolder.c_str(), NULL))
	//{
	//	// CopyFile(...)
	//	cout << "interfaceFiles made" << endl;
	//}
	//else
	//{
	//	cout << "interfaceFiles already exists" << endl;
	//	// Failed to create directory.
	//}

	//OutputFolder = "interfaceFiles/morphologies0";
	//if (CreateDirectory(OutputFolder.c_str(), NULL))
	//{
	//	// CopyFile(...)
	//	cout << "interfaceFiles made" << endl;
	//}
	//else
	//{
	//	cout << "interfaceFiles already exists" << endl;
	//	// Failed to create directory.
	//}

	if (startEvolution == true) {

		// Construct classes
		ER = unique_ptr<ER_VREP>(new ER_VREP);
		ER->settings = shared_ptr<Settings>(new Settings);

		// set all arguments
		// 1: seed and location
		int run = 0;
		simChar* arg1_param = simGetStringParameter(sim_stringparam_app_arg1);
		if (arg1_param != NULL) {
			run = atoi(arg1_param);
			simReleaseBuffer(arg1_param);
		}

		// 3: set the repository of the settings file. 
		simChar* arg3_param = simGetStringParameter(sim_stringparam_app_arg3);
		if (arg3_param != NULL) {
			ER->settings->setRepository(arg3_param);
			simReleaseBuffer(arg3_param);
		}
		else {
			std::cout << "Argument 3 was NULL" << endl;
		}
		// Read the settings file
		ER->settings->sceneNum = run; // sceneNum and seed can be overridden when specified in settings file. Code below will just ensure it is set to run. TODO
		ER->settings->readSettings();

		// Override settings parameters if argument 2
		// 2: 
		simChar* arg2_param = simGetStringParameter(sim_stringparam_app_arg2);
		if (arg2_param != NULL) {
			const int arg2_param_i = atoi(arg2_param);

			if (arg2_param_i == 9)
			{
				ER->simSet = RECALLBEST;
				ER->settings->instanceType = ER->settings->INSTANCE_REGULAR;
				//ER->settings->morphologyType = ER->settings->MODULAR_PHENOTYPE;
			}
			else if (arg2_param_i == 1)
			{
				ER->settings->instanceType = ER->settings->INSTANCE_SERVER;
			}

			simReleaseBuffer(arg2_param);
		}

		// sceneNum and seed will not be utilized from the settings file anymore
		ER->settings->sceneNum = run;
		ER->settings->seed = run;
		ER->randNum = shared_ptr<RandNum>(new RandNum(run));

		// Actual initialization of ER
		ER->initialize();
		//	ER->environment->sceneLoader();
		if (ER->settings->verbose) {
			cout << "ER initialized" << endl;
		}
	}
	int signal[1] = { 0 };
	simSetIntegerSignal((simChar*) "simulationState", signal[0]);
	//cout << ER->ea->populationGenomes[0]->settings->COLOR_LSYSTEM << endl;

	return(7); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
	// version 1 was for V-REP versions before V-REP 2.5.12
	// version 2 was for V-REP versions before V-REP 2.6.0
	// version 5 was for V-REP versions before V-REP 3.1.0 
	// version 6 is for V-REP versions after V-REP 3.1.3
	// version 7 is for V-REP versions after V-REP 3.2.0 (completely rewritten)
}

VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
	unloadVrepLibrary(vrepLib); // release the library
}

void saveLog(int num) {
	ofstream logFile;
	logFile.open("timeLog" + std::to_string(num) +".csv", ios::app);
	clock_t now = clock();
//	double deltaSysTime = difftime((double) time(0), sysTime) ;
	int deltaSysTime = now - sysTime;
	logFile << "time for completing " << counter << " individuals = ," << deltaSysTime << endl;
	sysTime = clock() ;
	counter = 0; 
	logFile.close();
}

VREP_DLLEXPORT void* v_repMessage(int message, int* auxiliaryData, void* customData, int* replyData)
{
	static bool refreshDlgFlag = true;
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode, &errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode, sim_api_errormessage_ignore);
	void* retVal = NULL;

	/*if (GetAsyncKeyState(VK_SPACE) & 0x80000000) {
		if (initialized == true) {
			initialized = false;
		}
		else {
			initialized = true;
		}
	}
	*///if (initialized == true) {
	
	if (startEvolution == true) {
		if (message == sim_message_eventcallback_modulehandle) {
			ER->handleSimulation(); // handling the simulation. 
		}

		if (message == sim_message_eventcallback_simulationabouttostart) {
			//tStart = clock();
			ER->startOfSimulation();
		}

		if (initCall == true) {
			initCall = false;
			counter = 0;
			if (ER->settings->evolutionType != ER->settings->EMBODIED_EVOLUTION && atoi(simGetStringParameter(sim_stringparam_app_arg2)) != 9
				&& ER->settings->instanceType != ER->settings->INSTANCE_SERVER) {
				simStartSimulation();

			}
			if (atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 9) {
				// simStartSimulation();
			}
		}
		//	}
		int signal[1] = { 0 };
		simGetIntegerSignal((simChar*) "simulationState", signal);
		if (signal[0] == 99) {
			cout << "should quit the simulator" << endl;
			simQuitSimulator(true);
		}

		if (loadingPossible == true && ER->settings->instanceType == ER->settings->INSTANCE_SERVER && simGetSimulationState() == sim_simulation_stopped) {
			// wait until command is received
			if (signal[0] == 1) {
				simSetIntegerSignal((simChar*) "simulationState", 8);
				int sceneNumber[1] = { 0 };
				int individual[1] = { 0 };
				//		cout << "Repository should be files and is " << ER->settings->repository << endl;
				//simGetIntegerSignal((simChar*) "sceneNumber", sceneNumber); // sceneNumber is currently not used. 
				simGetIntegerSignal((simChar*) "individual", individual);
				if (ER->loadIndividual(individual[0]) == false) {
					simSetIntegerSignal((simChar*) "simulationState", 9); // 9 is now the error state
				}
				else {
					// old function:
					//ER->ea->loadIndividual(individual[0], sceneNumber[0]);
					//saveLog(1);
					//cout << "Not loaded: see this comment in code to adjust" << endl;
					simStartSimulation();
					loadingPossible = false;
				}
			}
		}
		else if (message == sim_message_eventcallback_simulationended) {
			initCall = true; // this restarts the simulation
		//	printf("Time taken: %.4fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
		//	if (timeCount % 10 == 0) {
			//	ofstream file;
			//	ostringstream fileName;
			//	fileName << "timeLog.txt";
			//	file.open(fileName.str(), ios::out | ios::ate | ios::app);
			//	file << "Time taken at " << timeCount << ": " << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;
			//	file.close();
		//	}	
			timeCount++;
			ER->endOfSimulation();
			loadingPossible = true;
		}
	}

	//	simSetIntegerParameter(sim_intparam_error_report_mode, errorModeSaved); // restore previous settings
	return(retVal);
}
