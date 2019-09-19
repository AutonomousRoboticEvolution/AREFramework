/**
	@file v_repExtER.cpp
    @authors Edgar Buchanan, Wei Li, Matteo de Carlo and Frank Veenstra
	@brief This files from the three arguments decides whether to start simulation in local or server mode,
    specifies seed and repository.
*/

#include "v_repExtER.h"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include "v_repLib.h"

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
    extern "C" {
    #include <unistd.h>
    }
	#define WIN_AFX_MANAGE_STATE
#endif /* __linux || __APPLE__ */

#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)	CONCAT(x,y,z)

LIBRARY vrepLib;

///save time log
void saveLog(int num)
{
	std::ofstream logFile;
	logFile.open("timeLog" + std::to_string(num) +".csv", std::ios::app);
	clock_t now = clock();
//	double deltaSysTime = difftime((double) time(0), sysTime) ;
	int deltaSysTime = now - sysTime;
	logFile << "time for completing " << counter << " individuals = ," << deltaSysTime << std::endl;
	sysTime = clock();
	counter = 0;
	logFile.close();
}

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

    startEvolution = true;
	if (startEvolution) {
		// Construct classes
		ER = std::unique_ptr<ER_VREP>(new ER_VREP);   //the class used to handle the EA
		ER->settings = std::shared_ptr<Settings>(new Settings);  //initialize settings in the constructor

		/// Set all three arguments
		/// 1: The first argument sets seed and location
		int run = 0; // evolutionary run
		simChar* arg1_param = simGetStringParameter(sim_stringparam_app_arg1);
		if (arg1_param != nullptr) {
			run = atoi(arg1_param);
			std::cout << "run is set to " << arg1_param << std::endl;
			simReleaseBuffer(arg1_param);
		}
		/// 3: The third argument sets the repository
		simChar* arg3_param = simGetStringParameter(sim_stringparam_app_arg3);
		if (arg3_param != nullptr) {
			ER->settings->setRepository(arg3_param);
			simReleaseBuffer(arg3_param);
		}
		else {
			std::cout << "Argument 3 was NULL" << std::endl;
		}
		// Read the settings file; specify the ID of experimental run
		ER->settings->sceneNum = run;	// sceneNum and seed can be overridden when specified in settings file. Code below will just ensure it is set to run. TODO
		ER->settings->readSettings();	// load the settings if the *.csv exists
        ER->settings->seed = run;       // these two lines need to be updated; the idea was to overwrite sceneNum abd seed
        ER->randNum = std::make_shared<RandNum>(run);  //used for generating random number using the seed

		/// 2: The second argument sets the condition for simulation
		simChar* arg2_param = simGetStringParameter(sim_stringparam_app_arg2);
        int arg2_param_i = -1;

		if (arg2_param != nullptr) {
            arg2_param_i = atoi(arg2_param);
            simReleaseBuffer(arg2_param);
        }

        switch (arg2_param_i) {
            /// Run EA in server-client mode
            case 1:
                ER->settings->startingCondition = ER->settings->COND_RUN_EVOLUTION_SERVER;
                ER->settings->instanceType = ER->settings->INSTANCE_SERVER;  //run EA in a client-server mode (can be parallel)
                break;
                /// Run EA in local mode
            case 2:
                ER->settings->startingCondition = ER->settings->COND_RUN_EVOLUTION_CLIENT;
                ER->startRun = true;
                break;
            case 7:
                ER->settings->instanceType = ER->settings->INSTANCE_REGULAR;
                ER->settings->morphologyType = ER->settings->MODULAR_PHENOTYPE;
                ER->settings->startingCondition = ER->settings->COND_LOAD_BEST;
                break;
            case 8:
                ER->settings->instanceType = ER->settings->INSTANCE_REGULAR;
                ER->settings->startingCondition = ER->settings->COND_LOAD_BEST;
                break;
                /// Load best individual
            case 9:
                ER->settings->instanceType = ER->settings->INSTANCE_REGULAR;  //run EA inside the plug-in untill termination condition is met
                ER->settings->startingCondition = ER->settings->COND_LOAD_BEST;
                break;
                /// Wrong argument or no second argument, don't run evolution
            default:
                std::cout << "No second argument so not running evolution" << std::endl;
                ER->startRun = false;
                break;
        }


		// Actual initialization of ER
		ER->initialize(); 
		//	ER->environment->sceneLoader();
		if (ER->settings->verbose) {
            std::cout << "ER initialized" << std::endl;
		}
	}
	int signal[1] = { 0 };
	simSetIntegerSignal((simChar*) "simulationState", signal[0]);  //Set the signal back to the client (ready to accecpt genome)
	//cout << ER->ea->populationGenomes[0]->settings->COLOR_LSYSTEM << endl;

	return(7); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
	// version 1 was for V-REP versions before V-REP 2.5.12
	// version 2 was for V-REP versions before V-REP 2.6.0
	// version 5 was for V-REP versions before V-REP 3.1.0
	// version 6 is for V-REP versions after V-REP 3.1.3
	// version 7 is for V-REP versions after V-REP 3.2.0 (completely rewritten)
}

// Release the v-rep lib
VREP_DLLEXPORT void v_repEnd()
{ // This is called just once, at the end of V-REP
	unloadVrepLibrary(vrepLib); // release the library
}

VREP_DLLEXPORT void* v_repMessage(int message, int* auxiliaryData, void* customData, int* replyData)
{
	static bool refreshDlgFlag = true;
	int errorModeSaved;
	simGetIntegerParameter(sim_intparam_error_report_mode, &errorModeSaved);
	simSetIntegerParameter(sim_intparam_error_report_mode, sim_api_errormessage_ignore);
	void* retVal = nullptr;

	if (ER->startRun) {
		if (message == sim_message_eventcallback_simulationabouttostart) {
			// tStart = clock();
			// Initializes population
			ER->startOfSimulation();  //start from here after simStartSimulation is called
			if (ER->settings->verbose) {
                std::cout << "SIMULATION ABOUT TO START" << std::endl;
			}
		}
		else if (message == sim_message_eventcallback_modulehandle) {

			ER->handleSimulation(); // handling the simulation.
			timeCount = 0;
		}
		else if (message == sim_message_eventcallback_simulationended) {
			initCall = true; // this tells the main loop to restart the simulation
			ER->endOfSimulation();
			loadingPossible = true;  // start another simulation
			if (ER->settings->verbose) {
                std::cout << "SIMULATION ENDED" << std::endl;
			}
		}
		//TODO you probably meant ==?
		if (message = sim_message_eventcallback_modulehandle) {
			timeCount++;  //need to wait a few time steps to start a new simulation
			if (ER->settings->verbose) {
				//cout << timeCount << endl;
			}
		}

		if (initCall && timeCount > 10) {
            timeCount = 0;
            initCall = false;
            counter = 0;
            if (ER->settings->evolutionType != ER->settings->EMBODIED_EVOLUTION
                && ER->settings->startingCondition != ER->settings->COND_LOAD_BEST
                && ER->settings->instanceType != ER->settings->INSTANCE_SERVER
                && ER->settings->startingCondition != ER->settings->COND_LOAD_SPECIFIC_INDIVIDUAL) {
                simStartSimulation();
            }
            if (ER->settings->startingCondition == ER->settings->COND_LOAD_BEST) {
                // simStartSimulation();
                if (ER->settings->verbose){
                    std::cout<< "Load best individual" << std::endl;
                }
            }

            // TODO:
            //if (simulationSettings == 8) { // load specific genotype
            //
            //}
            //if (simulationSettings == 7) { // load specific phenotype
            //
            //}

        }
		// client and v-rep plugin communicates using signal and remote api
		int signal[1] = { 0 };
		int returnVal = simGetIntegerSignal((simChar*) "simulationState", signal);
		simGetIntegerSignal((simChar*) "simulationState", signal);
		if (signal[0] == 99) {
            std::cout << "should quit the simulator" << std::endl;
			simQuitSimulator(true);
		}
		else if (loadingPossible && ER->settings->instanceType == ER->settings->INSTANCE_SERVER && simGetSimulationState() == sim_simulation_stopped) {
			// time out when not receiving commands for 5 minutes.
			if (!timerOn) {
				sysTime = clock();
				timeElapsed = 0;
				timerOn = true;
			}
			else {
				// printf("Time taken: %.4fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
				timeElapsed = (double)(clock() - sysTime) / CLOCKS_PER_SEC;
				if (timeElapsed > 300) {
					std::cout << "Didn't receive a signal for 5 minutes. Shutting down server " << std::endl;
					simQuitSimulator(true);
				}
			}

			// wait until command is received
			if (signal[0] == 1) {
				timerOn = false;
				simSetIntegerSignal((simChar*) "simulationState", 8);
				int sceneNumber[1] = { 0 };
				int individual[1] = { 0 };
				//		cout << "Repository should be files and is " << ER->settings->repository << endl;
				//simGetIntegerSignal((simChar*) "sceneNumber", sceneNumber); // sceneNumber is currently not used.

				simGetIntegerSignal((simChar*) "individual", individual);
				if (not ER->loadIndividual(individual[0])) {
					if (ER->settings->verbose) {
						std::cout << "Server here, I could not load the specified individual: " << individual[0] << std::endl;
						std::cout << "My signal was " << signal[0] << std::endl;
					}
					simSetIntegerSignal((simChar*) "simulationState", 9); // 9 is now the error state
				}
				else {
					// old function:
					//ER->ea->loadIndividual(individual[0], sceneNumber[0]);
					//saveLog(1);
					//cout << "Not loaded: see this comment in code to adjust" << endl;
					simStartSimulation();   //the genome is loaded succussully; start a simulation and load a genome for evaluation
					loadingPossible = false;
				}
			}
		}

	}

	//	simSetIntegerParameter(sim_intparam_error_report_mode, errorModeSaved); // restore previous settings
	return(retVal);
}
