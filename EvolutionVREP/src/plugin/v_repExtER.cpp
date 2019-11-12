/**
	@file v_repExtER.cpp
    @authors Edgar Buchanan, Wei Li, Matteo de Carlo and Frank Veenstra
	@brief This files from the three arguments decides whether to start simulation in local or server mode,
    specifies seed and repository.
    @details The plug-in to run needs three arguments: -gX, -gX and gX
    - The first argument defines the seed of the experiment.
    - The second argument defines how the experiment is run.
    * 1 -> Run standard EA in client-server mode
    * 2 -> Run standard EA in local mode
    * 7 ->
    * 8 ->
    * 9 -> Load the best individual from the last generation.
    - The third argumt defines where the repository is
*/

#include "plugin/v_repExtER.h"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include <memory>
#include "v_repLib.h"

namespace are_sett = are::settings;

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
{
    std::cout << "---------------------------" << std::endl
              << "STARTING WITH ARE FRAMEWORK" << std::endl
              << "---------------------------" << std::endl;


    // This is called just once, at the start of V-REP.
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
        ERVREP = std::make_unique<are::ER>();   // The class used to handle the EA

        are_sett::ParametersMapPtr parameters = std::make_shared<are_sett::ParametersMap>(
                are_sett::loadParameters(simGetStringParameter(sim_stringparam_app_arg2)));

        bool verbose = are_sett::getParameter<are_sett::Boolean>(parameters,"#verbose").value;

      //  settings->setRepository(simGetStringParameter(sim_stringparam_app_arg3));
//        settings->sceneNum = 0;
        std::cout << "readSettings" << std::endl;
        //settings->readSettings();
//        if(are_sett::cast<are_sett::String>(parameters->at("#loadExtSettings")))
//            if(!load_class_exp_plugin<are::Settings>(settings,settings->exp_plugin_name,"create_settings"))
//                exit(1);

        if(verbose){
            std::cout << "---------------------------------" << std::endl;
            std::cout << "loading experiment : " << are_sett::getParameter<are_sett::String>(parameters,"#expPluginName").value << std::endl;
            std::cout << "---------------------------------" << std::endl;
        }

        ERVREP->set_parameters(parameters);  // Initialize settings in the constructor
        ERVREP->set_randNum(std::make_shared<misc::RandNum>(0)); //todo change


		/// Set all three arguments
		/// 1: The first argument sets seed and location
//		int run = 0; // evolutionary run
//		simChar* arg1_param = simGetStringParameter(sim_stringparam_app_arg1);
//		if (arg1_param != nullptr) {
//			run = atoi(arg1_param);
//            if(verbose){
//                std::cout << "Run is set to " << arg1_param << std::endl;
//			}
//			simReleaseBuffer(arg1_param);
//		}
//		/// 3: The third argument sets the repository
//		simChar* arg3_param = simGetStringParameter(sim_stringparam_app_arg3);
//		if (arg3_param != nullptr)
//			simReleaseBuffer(arg3_param);
//		else {
//			std::cout << "Argument 3 was NULL" << std::endl;
//		}
//		// Read the settings file; specify the ID of experimental run
//        ERVREP->get_settings()->sceneNum = run;	// sceneNum and seed can be overridden when specified in settings file. Code below will just ensure it is set to run. TODO
//        ERVREP->get_settings()->readSettings();	// load the settings if the *.csv exists
//        ERVREP->get_settings()->seed = run;       // these two lines need to be updated; the idea was to overwrite sceneNum abd seed
//      //used for generating random number using the seed

		/// 2: The second argument sets the condition for simulation
//		simChar* arg2_param = simGetStringParameter(sim_stringparam_app_arg2);
//        int arg2_param_i = -1;

//		if (arg2_param != nullptr) {
//            arg2_param_i = atoi(arg2_param);
//            simReleaseBuffer(arg2_param);
//        }

//        switch (arg2_param_i) {
//            case 1: /// Run EA in server-client mode
//                if(verbose){
//                    std::cout << "Mode: Server-client EA" << std::endl;
//                }
//                ERVREP->get_settings()->startingCondition = are::Settings::COND_RUN_EVOLUTION_SERVER;
//                ERVREP->get_settings()->instanceType = are::Settings::INSTANCE_SERVER;
//                break;
//            case 2: /// Run EA in local mode
//                if(ERVREP->get_settings()->verbose){
//                    std::cout << "Mode: Local EA" << std::endl;
//                }
//                ERVREP->get_settings()->startingCondition = are::Settings::COND_RUN_EVOLUTION_CLIENT;
//                ERVREP->get_settings()->instanceType = are::Settings::INSTANCE_REGULAR;
//                ERVREP->set_startRun(true);
//                break;
//            case 7:
//                if(ERVREP->get_settings()->verbose){
//                    std::cout << "?" << std::endl;
//                }
//                ERVREP->get_settings()->instanceType = are::Settings::INSTANCE_REGULAR;
//                ERVREP->get_settings()->morphologyType = are::Settings::MODULAR_PHENOTYPE;
//                ERVREP->get_settings()->startingCondition = are::Settings::COND_LOAD_BEST;
//                break;
//            case 8:
//                if(ERVREP->get_settings()->verbose){
//                    std::cout << "Mode: ?" << std::endl;
//                }
//                ERVREP->get_settings()->instanceType = are::Settings::INSTANCE_REGULAR;
//                ERVREP->get_settings()->startingCondition = are::Settings::COND_LOAD_BEST;
//                break;
//            case 9: /// Load best individual
//                if(ERVREP->get_settings()->verbose){
//                    std::cout << "Mode: Load best individual ever" << std::endl;
//                }
//                ERVREP->get_settings()->instanceType = are::Settings::INSTANCE_REGULAR;  //run EA inside the plug-in untill termination condition is met
//                ERVREP->get_settings()->startingCondition = are::Settings::COND_LOAD_BEST;
//                break;

//            default: /// Wrong argument or no second argument, don't run evolution
//                if(ERVREP->get_settings()->verbose){
//                    std::cout << "Mode: No second argument, so not running evolution" << std::endl;
//                }
//                ERVREP->set_startRun(false);
//                break;
//        }
		// Actual initialization of ER
		ERVREP->initialize();
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

    are_sett::ParametersMap param = (*ERVREP->get_parameters());
    bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;

    if (not ERVREP->get_startRun()) {
        return nullptr;
    }

    int errorModeSaved;
    simGetIntegerParameter(sim_intparam_error_report_mode, &errorModeSaved);
    simSetIntegerParameter(sim_intparam_error_report_mode, sim_api_errormessage_ignore);
    void *retVal = nullptr;

    // ABOUT TO START
    if (message == sim_message_eventcallback_simulationabouttostart)
    {
        assert(simulationState == STARTING);
        simulationState = BUSY;
        // Initializes population
        ERVREP->startOfSimulation();  //start from here after simStartSimulation is called
        if (verbose) {
            std::cout << "SIMULATION ABOUT TO START" << std::endl;
        }
    }
    // MODULE_HANDLE (DEPRECATED)
    else if (message == sim_message_eventcallback_modulehandle)
    {
        assert(simulationState == BUSY);
        ERVREP->handleSimulation(); // handling the simulation.
    }
    // SIMULATION ENDED
    else if (message == sim_message_eventcallback_simulationended)
    {
        assert(simulationState == BUSY);
        simulationState = CLEANUP;
        ERVREP->endOfSimulation();
        loadingPossible = true;  // start another simulation
        if (verbose) {
            std::cout << "SIMULATION ENDED" << std::endl;
        }
    }

    if (simulationState == CLEANUP) {
        timeCount++;  //need to wait a few time steps to start a new simulation
    }

    if (simulationState == CLEANUP && timeCount > 10) {
        simulationState = FREE;
        timeCount = 0;
    }

    // START NEW SIMULATION
    if (simulationState == FREE)
    {
        simulationState = STARTING;
        counter = 0;
        if (are_sett::getParameter<are_sett::Integer>(param,"#instanceType").value != are_sett::INSTANCE_SERVER)
            simStartSimulation();
    }

    // client and v-rep plugin communicates using signal and remote api
    int signal[1] = { 0 };
    int returnVal = simGetIntegerSignal((simChar*) "simulationState", signal);
    simGetIntegerSignal((simChar*) "simulationState", signal);
    if (signal[0] == 99)
    {
        std::cout << "should quit the simulator" << std::endl;
        simQuitSimulator(true);
    }
    else if (simulationState == FREE
          && are_sett::getParameter<are_sett::Integer>(param,"#instanceType").value == are_sett::INSTANCE_SERVER
          && simGetSimulationState() == sim_simulation_stopped)
    {
        // time out when not receiving commands for 5 minutes.
        if (!timerOn) {
            sysTime = clock();
            timeElapsed = 0;
            timerOn = true;
        } else {
            // printf("Time taken: %.4fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
            timeElapsed = (double) (clock() - sysTime) / CLOCKS_PER_SEC;
            if (timeElapsed > 300)
            {
                std::cout << "Didn't receive a signal for 5 minutes. Shutting down server " << std::endl;
                simQuitSimulator(true);
            }
        }

        // wait until command is received
        if (signal[0] == 1)
        {
            timerOn = false;
            simSetIntegerSignal((simChar *) "simulationState", 8);
            int sceneNumber[1] = {0};
            int individual[1] = {0};
            //		cout << "Repository should be files and is " << ER->settings->repository << endl;
            //simGetIntegerSignal((simChar*) "sceneNumber", sceneNumber); // sceneNumber is currently not used.

            simGetIntegerSignal((simChar *) "individual", individual);
            if (not ERVREP->loadIndividual(individual[0])) {
                if (verbose) {
                    std::cout << "Server here, I could not load the specified individual: " << individual[0]
                              << std::endl;
                    std::cout << "My signal was " << signal[0] << std::endl;
                }
                simSetIntegerSignal((simChar *) "simulationState", 9); // 9 is now the error state
            } else {
                // old function:
                //ER->ea->loadIndividual(individual[0], sceneNumber[0]);
                //saveLog(1);
                //cout << "Not loaded: see this comment in code to adjust" << endl;
                simStartSimulation();   //the genome is loaded succussully; start a simulation and load a genome for evaluation
                loadingPossible = false;
            }
        }
    }

	//	simSetIntegerParameter(sim_intparam_error_report_mode, errorModeSaved); // restore previous settings
	return retVal;
}
