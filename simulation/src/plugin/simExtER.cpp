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

#include "simulatedER/simExtER.h"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include <memory>
#include "simLib.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <random>

namespace are_sett = are::settings;
namespace are_c = are::client;

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

LIBRARY simLib;

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

SIM_DLLEXPORT unsigned char simStart(void* reservedPointer, int reservedInt)
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


//    // Check the V-REP version:
//    int vrepVer;
//    simGetIntegerParameter(sim_intparam_program_version, &vrepVer);
//    if (vrepVer < 30200) // if V-REP version is smaller than 3.02.00
//    {
//        std::cout << "Sorry, your V-REP copy is somewhat old, V-REP 3.2.0 or higher is required. Cannot start 'BubbleRob' plugin.\n";


//        unloadSimLibrary(simLib);


//        return(0); // Means error, V-REP will unload this plugin
//    }

    are_sett::ParametersMapPtr parameters = std::make_shared<are_sett::ParametersMap>(
                are_sett::loadParameters(simGetStringParameter(sim_stringparam_app_arg1)));
    int instance_type = are_sett::getParameter<are_sett::Integer>(parameters,"#instanceType").value;
    bool verbose = are_sett::getParameter<are_sett::Boolean>(parameters,"#verbose").value;
    int seed = are_sett::getParameter<are_sett::Integer>(parameters,"#seed").value;

    if(verbose){
        if(instance_type == are_sett::INSTANCE_REGULAR)
            std::cout << "Starting in local intance mode" << std::endl;
        else if(instance_type == are_sett::INSTANCE_SERVER)
            std::cout << "Starting in server intance mode" << std::endl;
    }

    simulationState = INITIALIZING;
    // Construct classes
    ERVREP = std::make_unique<are::sim::ER>();   // The class used to handle the EA

    if(verbose){
        std::cout << "Parameters Loaded" << std::endl;
        std::cout << "---------------------------------" << std::endl;
        std::cout << "loading experiment : " << are_sett::getParameter<are_sett::String>(parameters,"#expPluginName").value << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }
    are_sett::Property::Ptr properties(new are_sett::Property);
    ERVREP->set_properties(properties);
    ERVREP->set_parameters(parameters);  // Initialize settings in the constructor
    if(seed < 0){
        std::random_device rd;
        seed = rd();
        are_sett::random::parameters->emplace("#seed",new are_sett::Integer(seed));
    }
    are::misc::RandNum rn(seed);
    ERVREP->set_randNum(std::make_shared<are::misc::RandNum>(rn));
    ERVREP->initialize();
    simulationState = FREE;
    properties.reset();

    if(instance_type == are_sett::INSTANCE_REGULAR){
        //Write parameters in the log folder.
        are_sett::saveParameters(are::Logging::log_folder + std::string("/parameters.csv"),parameters);
    }

    if(instance_type == are_sett::INSTANCE_SERVER)
    {
        int signal[1] = { 0 };
        simSetIntegerSignal((simChar*) "simulationState", signal[0]);  //Set the signal back to the client (ready to accecpt genome)
    }
    //cout << ER->ea->populationGenomes[0]->settings->COLOR_LSYSTEM << endl;

    return(9); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
    // version 1 was for CoppeliaSim versions before CoppeliaSim 2.5.12
    // version 2 was for CoppeliaSim versions before CoppeliaSim 2.6.0
    // version 5 was for CoppeliaSim versions before CoppeliaSim 3.1.0
    // version 6 is for CoppeliaSim versions after CoppeliaSim 3.1.3
    // version 7 is for CoppeliaSim versions after CoppeliaSim 3.2.0 (completely rewritten)
    // version 8 is for CoppeliaSim versions after CoppeliaSim 3.3.0 (using stacks for data exchange with scripts)
    // version 9 is for CoppeliaSim versions after CoppeliaSim 3.4.0 (new API notation)
}

// Release the v-rep lib
SIM_DLLEXPORT void simEnd()
{ // This is called just once, at the end of V-REP
    unloadSimLibrary(simLib);

}

SIM_DLLEXPORT void* simMessage(int message, int* auxiliaryData, void* customData, int* replyData)
{
    are_sett::ParametersMapPtr param = ERVREP->get_parameters();
    //    bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;
    int instanceType = are_sett::getParameter<are_sett::Integer>(param,"#instanceType").value;

    if(instanceType == are_sett::INSTANCE_REGULAR)
    {
        localMessageHandler(message);
    }
    else if(instanceType == are_sett::INSTANCE_SERVER)
    {
        clientMessageHandler(message);
    }
    param.reset();
}

void localMessageHandler(int message){
    are_sett::ParametersMap param = (*ERVREP->get_parameters());
    bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;

    int errorModeSaved;
    simGetIntegerParameter(sim_intparam_error_report_mode, &errorModeSaved);
    simSetIntegerParameter(sim_intparam_error_report_mode, sim_api_errormessage_ignore);

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
    //Runing Simulation
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
    if (simulationState == FREE && ERVREP->get_ea()->get_population().size() > 0)
    {
        simulationState = STARTING;
        counter = 0;
        simStartSimulation();
    }else if(simulationState == FREE && ERVREP->get_ea()->get_population().size() == 0){
        ERVREP->endOfSimulation();
    }
}

void clientMessageHandler(int message){

    if(message == sim_message_eventcallback_modelloaded)
        return;

    are_sett::ParametersMap param = (*ERVREP->get_parameters());
    bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;

    // client and v-rep plugin communicates using signal and remote api
    int clientState[1] = {10111};
    simGetIntegerSignal((simChar*) "clientState", clientState);

    if (simulationState == FREE
            && simGetSimulationState() == sim_simulation_stopped)
    {
        simSetIntegerSignal("simulationState",are_c::IDLE);

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

    }

    // ABOUT TO START
    if (message == sim_message_eventcallback_simulationabouttostart)
    {
        simulationState = BUSY;

        if (verbose) {
            std::cout << "SIMULATION ABOUT TO START" << std::endl;
        }
        //        simStartSimulation();
        ERVREP->initEnv();
        ERVREP->initIndividual();//startOfSimulation();
        // Initializes population
        simSetIntegerSignal("simulationState",are_c::BUSY);
        simSetFloatSignal("simulationTime",0);
        //        extApi_sleepMs(50);

    }
    //Runing Simulation
    else if (message == sim_message_eventcallback_modulehandle) //&& clientState[0] == BUSY)
    {
        ERVREP->handleSimulation(); // handling the simulation.
        simSetIntegerSignal("simulationState",are_c::BUSY);
    }\
    // SIMULATION ENDED
    else if (message == sim_message_eventcallback_simulationended)
    {
        simulationState = CLEANUP;
        ERVREP->endOfSimulation();
        std::cout << ERVREP->get_evalIsFinish() << std::endl;
        if(ERVREP->get_evalIsFinish()){

            std::string indString = ERVREP->get_currentInd()->to_string();
            simSetStringSignal("currentInd",indString.c_str(),indString.size());
            simSetIntegerSignal("simulationState",are_c::FINISH);

            //loadingPossible = true;  // start another simulation
            if (verbose) {
                std::cout << "EVALUATION ENDED" << std::endl;
            }
        }
        else{
            // Initializes population
            if (verbose) {
                std::cout << "SIMULATION ENDED" << std::endl;
            }
            simSetIntegerSignal("simulationState",are_c::RESTART);
            simulationState = RESTART;
            std::string indString = ERVREP->get_currentInd()->to_string();
            simSetStringSignal("currentInd",indString.c_str(),indString.size());
            loadingPossible = true;  // start another simulation
            return;
       }

//        std::string indString = ERVREP->get_currentInd()->to_string();
//        simSetStringSignal("currentInd",indString.c_str(),indString.size());
//        simSetIntegerSignal("simulationState",are_c::FINISH);

//        if (verbose) {
//            std::cout << "SIMULATION ENDED" << std::endl;
//        }
    }

    if (clientState[0] == are_c::IDLE)
    {
        timerOn = false;
        simulationState = STARTING;
        simSetIntegerSignal("simulationState",are_c::READY);
    }else if(clientState[0] == are_c::READY && (simulationState == STARTING || simulationState == RESTART)){
        simStartSimulation();
    }
    else if(clientState[0] == 99){
        std::cout << "Stop Instance !" << std::endl;
        simQuitSimulator(true);
    }

}

