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

#include "simulatedER/v_repExtER.h"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include <memory>

#include "v_repLib.h"
#include "remoteApi/extApi.h"

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
int timeout = 60;


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




    simLib = loadVrepLibrary(temp.c_str());
    if (simLib == NULL)
    {
        std::cout << "Error, could not find or correctly load v_rep.dll. Cannot start 'BubbleRob' plugin.\n";
        return(0); // Means error, V-REP will unload this plugin
    }

    if (getVrepProcAddresses(simLib) == 0)
    {
        std::cout << "Error, could not find all required functions in v_rep.dll. Cannot start 'BubbleRob' plugin.\n";

        unloadVrepLibrary(simLib); // release the library

        return(0); // Means error, V-REP will unload this plugin
    }


    // Check the V-REP version:
    int vrepVer;
    simGetIntegerParameter(sim_intparam_program_version, &vrepVer);
    if (vrepVer < 30200) // if V-REP version is smaller than 3.02.00
    {
        std::cout << "Sorry, your V-REP copy is somewhat old, V-REP 3.2.0 or higher is required. Cannot start 'BubbleRob' plugin.\n";
        unloadVrepLibrary(simLib); // release the library
        return(0); // Means error, V-REP will unload this plugin
    }

    are_sett::defaults::parameters->emplace("#evaluationOrder",std::make_shared<are_sett::Integer>(1)); //Default first in last out

    simChar* parameters_filepath = simGetStringParameter(sim_stringparam_app_arg1);
    are_sett::ParametersMapPtr parameters = std::make_shared<are_sett::ParametersMap>(
                are_sett::loadParameters(parameters_filepath));
    simReleaseBuffer(parameters_filepath);
    instance_type = are_sett::getParameter<are_sett::Integer>(parameters,"#instanceType").value;
    verbose = are_sett::getParameter<are_sett::Boolean>(parameters,"#verbose").value;
    int seed = are_sett::getParameter<are_sett::Integer>(parameters,"#seed").value;
    timeout += are_sett::getParameter<are_sett::Float>(parameters,"#maxEvalTime").value;

    //get simulator port
    std::string argument = simGetStringParameter(sim_stringparam_app_arg2);
    if(!argument.empty()){
        std::vector<std::string> split_arg;
        are::misc::split_line(argument,"_",split_arg);
        std::cout << split_arg[1] << std::endl;
        parameters->emplace("#port",std::make_shared<are_sett::String>(split_arg[1]));
    }

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

    ERVREP->set_parameters(parameters);  // Initialize settings in the constructor
    if(seed == -1){
        std::random_device rd;
        seed = rd();
        are_sett::random::parameters->emplace("#seed",std::make_shared<are_sett::Integer>(seed));
    }
    are::misc::RandNum rn(seed);
    ERVREP->set_randNum(std::make_shared<are::misc::RandNum>(rn));
    ERVREP->initialize();
    simulationState = CLEANUP;

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
    unloadVrepLibrary(simLib); // release the library
}

VREP_DLLEXPORT void* v_repMessage(int message, int* auxiliaryData, void* customData, int* replyData)
{
    //are_sett::ParametersMapPtr param = ERVREP->get_parameters();
    //    bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;
  //  int instanceType = are_sett::getParameter<are_sett::Integer>(param,"#instanceType").value;

    if(instance_type == are_sett::INSTANCE_REGULAR)
    {
        localMessageHandler(message);
    }
    else if(instance_type == are_sett::INSTANCE_SERVER)
    {
        clientMessageHandler(message);
    }
    //param.reset();
    return NULL;
}

void localMessageHandler(int message){
   // are_sett::ParametersMap param = (*ERVREP->get_parameters());
   // bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;

//    int errorModeSaved;
//    simGetIntegerParameter(sim_intparam_error_report_mode, &errorModeSaved);
//    simSetIntegerParameter(sim_intparam_error_report_mode, sim_api_errormessage_ignore);


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

    if(are::Logging::log_folder.empty()){
        simInt length;
        simChar* log_folder = simGetStringSignal((simChar*) "log_folder", &length);
        if(log_folder != nullptr){
            are::Logging::log_folder = std::string(log_folder);
            are::Logging::log_folder.resize(length);
        }else std::cerr << "ARE Warning: log folder not retrieved from client" << std::endl;
    }

    if(message == sim_message_eventcallback_modelloaded)
        return;

   // are_sett::ParametersMap param = (*ERVREP->get_parameters());
//    bool verbose = are_sett::getParameter<are_sett::Boolean>(param,"#verbose").value;

    // client and v-rep plugin communicates using signal and remote api
    int clientState[1] = {10111};
    int ret = simGetIntegerSignal((simChar*) "clientState", clientState);


    if (simulationState == FREE || ret < 1)
            //&& simGetSimulationState() == sim_simulation_stopped)
    {
        simSetIntegerSignal("simulationState",are_c::IDLE);

        // time out when not receiving commands for 5 minutes.
        if (!timerOn) {
            sysTime = std::chrono::system_clock::now();
            timeElapsed = 0;
            timerOn = true;
        } else {
            // printf("Time taken: %.4fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
            timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - sysTime).count();
            if (timeElapsed > timeout)
            {
                std::cout << "Didn't receive a signal for " << timeout <<  " seconds. Shutting down server " << std::endl;
                simQuitSimulator(true);
                exit(0);
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
            std::cout << "size of message sent " << indString.size() << std::endl;
            std::string req;
            are::send_string_no_reply(indString,ERVREP->get_ind_channel(),"ind ");
            std::cout << "send individual" << std::endl;

//            std::cout << "return value from simsetstringsignal " << simSetStringSignal("currentInd",indString.c_str(),indString.size()) << std::endl;
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
            simSetIntegerSignal("simulationState",are_c::BUSY);
            simulationState = RESTART;
            //std::string indString = ERVREP->get_currentInd()->to_string();
           // std::string req;
          //  are::send_string_no_reply(indString,ERVREP->get_ind_channel(),"ind ");
           // simSetStringSignal("currentInd",indString.c_str(),indString.size());
            loadingPossible = true;  // start another simulation
            return;
       }
    }

    if (simulationState == CLEANUP) {
        timeCount++;  //need to wait a few time steps to start a new simulation
    }

    if (simulationState == CLEANUP && timeCount > 10) {
        simulationState = FREE;
        timeCount = 0;
    }


    if (clientState[0] == are_c::IDLE)
    {
        simulationState = STARTING;
        simSetIntegerSignal("simulationState",are_c::READY);
    }else if(clientState[0] == are_c::READY && (simulationState == STARTING || simulationState == RESTART)){
        timerOn = false;
        simStartSimulation();
    }
    else if(clientState[0] == 99){
        std::cout << "Stop Instance !" << std::endl;
        simQuitSimulator(true);
        exit(0);
    }

}
