#include "simExtER.h"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include <memory>
#include "simLib.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <random>

//namespace are_sett = are::settings;
//namespace are_c = are::client;

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

static LIBRARY simLib;

///save time log
void saveLog(int num)
{
    std::ofstream logFile;
    logFile.open("timeLog" + std::to_string(num) +".csv", std::ios::app);
    clock_t now = clock();
    //	double deltaSysTime = difftime((double) time(0), sysTime) ;
//    int deltaSysTime = now - sysTime;
//    logFile << "time for completing " << counter << " individuals = ," << deltaSysTime << std::endl;
//    sysTime = clock();
//    counter = 0;
    logFile.close();
}

SIM_DLLEXPORT unsigned char simStart(void* reservedPointer, int reservedInt)
{
    std::cout << "---------------" << std::endl
              << "STARTING PLUGIN" << std::endl
              << "---------------" << std::endl;


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
    temp += "\\coppeliaSim.dll";
#elif defined (__linux)
    temp += "/libcoppeliaSim.so";
#elif defined (__APPLE__)
    temp += "/libcoppeliaSim.dylib";
#endif /* __linux || __APPLE__ */

    simLib = loadSimLibrary(temp.c_str());
    if (simLib == NULL)
    {
        std::cout << "Error, could not find or correctly load v_rep.dll. Cannot start 'BubbleRob' plugin.\n";
        return(0); // Means error, V-REP will unload this plugin
    }


    if (getSimProcAddresses(simLib) == 0)
    {
        std::cout << "Error, could not find all required functions in v_rep.dll. Cannot start 'BubbleRob' plugin.\n";

        unloadSimLibrary(simLib); // release the library

        return(0); // Means error, V-REP will unload this plugin
    }


    // Check the V-REP version:
    int vrepVer;
    simGetIntegerParameter(sim_intparam_program_version, &vrepVer);
    if (vrepVer < 30200) // if V-REP version is smaller than 3.02.00
    {
        std::cout << "Sorry, your V-REP copy is somewhat old, V-REP 3.2.0 or higher is required. Cannot start 'BubbleRob' plugin.\n";


        unloadSimLibrary(simLib);


        return(0); // Means error, V-REP will unload this plugin
    }

    return(10); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
    // version 1 was for CoppeliaSim versions before CoppeliaSim 2.5.12
    // version 2 was for CoppeliaSim versions before CoppeliaSim 2.6.0
    // version 5 was for CoppeliaSim versions before CoppeliaSim 3.1.0
    // version 6 is for CoppeliaSim versions after CoppeliaSim 3.1.3
    // version 7 is for CoppeliaSim versions after CoppeliaSim 3.2.0 (completely rewritten)
    // version 8 is for CoppeliaSim versions after CoppeliaSim 3.3.0 (using stacks for data exchange with scripts)
    // version 9 is for CoppeliaSim versions after CoppeliaSim 3.4.0 (new API notation)
    // version 10 is for CoppeliaSim versions after CoppeliaSim 4.1.0 (threads via coroutines)
}

// Release the v-rep lib
SIM_DLLEXPORT void simEnd()
{ // This is called just once, at the end of V-REP
    std::cout << "---" << std::endl
              << "END" << std::endl
              << "---" << std::endl;
    unloadSimLibrary(simLib);
}

SIM_DLLEXPORT void* simMessage(int message, int* auxiliaryData, void* customData, int* replyData)
{
//    std::cout << "-------" << std::endl
//              << "MESSAGE" << std::endl
//              << "-------" << std::endl;
    localMessageHandler(message);
//    clientMessageHandler(message);
}

void localMessageHandler(int message){

    int errorModeSaved;
    simGetIntegerParameter(sim_intparam_error_report_mode, &errorModeSaved);
    simSetIntegerParameter(sim_intparam_error_report_mode, sim_api_errormessage_ignore);

    // ABOUT TO START
    if (message == sim_message_eventcallback_simulationabouttostart)
    {
        assert(simulationState == STARTING);
        simulationState = BUSY;
    }
    //Runing Simulation
    else if (message == sim_message_eventcallback_modulehandle)
    {
        assert(simulationState == BUSY);
    }
    // SIMULATION ENDED
    else if (message == sim_message_eventcallback_simulationended)
    {
        assert(simulationState == BUSY);
        simulationState = CLEANUP;
//        loadingPossible = true;  // start another simulation
    }

    if (simulationState == CLEANUP) {
//        timeCount++;  //need to wait a few time steps to start a new simulation
    }

//    if (simulationState == CLEANUP && timeCount > 10) {
//        simulationState = FREE;
//        timeCount = 0;
//    }

    // START NEW SIMULATION
    if (simulationState == FREE)
    {
        simulationState = STARTING;
//        counter = 0;
        simStartSimulation();
    }
}

void clientMessageHandler(int message){

    if(message == sim_message_eventcallback_modelloaded)
        return;

//    are_sett::ParametersMap param = (*ERVREP->get_parameters());
//    bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;
//
//    // client and v-rep plugin communicates using signal and remote api
//    int clientState[1] = {10111};
//    simGetIntegerSignal((simChar*) "clientState", clientState);
//
//    if (simulationState == FREE
//            && simGetSimulationState() == sim_simulation_stopped)
//    {
//        simSetIntegerSignal("simulationState",are_c::IDLE);
//
//        // time out when not receiving commands for 5 minutes.
//        if (!timerOn) {
//            sysTime = clock();
//            timeElapsed = 0;
//            timerOn = true;
//        } else {
//            // printf("Time taken: %.4fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
//            timeElapsed = (double) (clock() - sysTime) / CLOCKS_PER_SEC;
//            if (timeElapsed > 300)
//            {
//                std::cout << "Didn't receive a signal for 5 minutes. Shutting down server " << std::endl;
//                simQuitSimulator(true);
//            }
//        }
//
//        // wait until command is received
//
//    }
//
//    // ABOUT TO START
//    if (message == sim_message_eventcallback_simulationabouttostart)
//    {
//        simulationState = BUSY;
//
//        if (verbose) {
//            std::cout << "SIMULATION ABOUT TO START" << std::endl;
//        }
//        //        simStartSimulation();
//        ERVREP->initEnv();
//        ERVREP->initIndividual();//startOfSimulation();
//        // Initializes population
//        simSetIntegerSignal("simulationState",are_c::BUSY);
//        simSetFloatSignal("simulationTime",0);
//        //        extApi_sleepMs(50);
//
//    }
//    //Runing Simulation
//    else if (message == sim_message_eventcallback_modulehandle) //&& clientState[0] == BUSY)
//    {
//        ERVREP->handleSimulation(); // handling the simulation.
//        simSetIntegerSignal("simulationState",are_c::BUSY);
//    }\
//    // SIMULATION ENDED
//    else if (message == sim_message_eventcallback_simulationended)
//    {
//        simulationState = CLEANUP;
//        ERVREP->endOfSimulation();
//        std::cout << ERVREP->get_evalIsFinish() << std::endl;
//        if(ERVREP->get_evalIsFinish()){
//
//            std::string indString = ERVREP->get_currentInd()->to_string();
//            simSetStringSignal("currentInd",indString.c_str(),indString.size());
//            simSetIntegerSignal("simulationState",are_c::FINISH);
//
//            //loadingPossible = true;  // start another simulation
//            if (verbose) {
//                std::cout << "EVALUATION ENDED" << std::endl;
//            }
//        }
//        else{
//            // Initializes population
//            if (verbose) {
//                std::cout << "SIMULATION ENDED" << std::endl;
//            }
//            simSetIntegerSignal("simulationState",are_c::RESTART);
//            simulationState = RESTART;
//            std::string indString = ERVREP->get_currentInd()->to_string();
//            simSetStringSignal("currentInd",indString.c_str(),indString.size());
//            loadingPossible = true;  // start another simulation
//            return;
//       }
//
////        std::string indString = ERVREP->get_currentInd()->to_string();
////        simSetStringSignal("currentInd",indString.c_str(),indString.size());
////        simSetIntegerSignal("simulationState",are_c::FINISH);
//
////        if (verbose) {
////            std::cout << "SIMULATION ENDED" << std::endl;
////        }
//    }
//
//    if (clientState[0] == are_c::IDLE)
//    {
//        timerOn = false;
//        simulationState = STARTING;
//        simSetIntegerSignal("simulationState",are_c::READY);
//    }else if(clientState[0] == are_c::READY && (simulationState == STARTING || simulationState == RESTART)){
//        simStartSimulation();
//    }
//    else if(clientState[0] == 99){
//        std::cout << "Stop Instance !" << std::endl;
//        simQuitSimulator(true);
//    }

}

