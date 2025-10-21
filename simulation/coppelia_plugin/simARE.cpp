#include "simARE.hpp"
#include "ARE/Logging.h"
#include <simLib/scriptFunctionData.h>
#include <simLib/simLib.h>
#include <simStack/stackArray.h>

static LIBRARY simLib;

void getLogFolder(SScriptCallBack* p){
    std::cout << "entering getLogFolder" << std::endl;
    int stack = p->stackID;
    CStackArray args;
    args.buildFromStack(stack); 
    if ( (args.getSize() > 0) && args.isString(0) )
    {
        are::Logging::set_log_folder(args.getString(0));
        std::cout << "log folder retrieved: " << are::Logging::log_folder << std::endl;
        CStackArray output;
        output.pushString("log folder received");
        output.buildOntoStack(stack);
    }
    else
        simSetLastError(nullptr, "Not enough arguments or wrong arguments.");
}

void initEnvironment(SScriptCallBack* p){
    std::cout << "entering initEnvironment" << std::endl;
    int stack = p->stackID;

    ER->initEnv();
    std::cout << "environment initialised" << std::endl;
    CStackArray output;
    output.pushString("environment initialised");
    output.buildOntoStack(stack);

    std::cout << "Exiting initEnvironment" << std::endl;
}

void spawnRobot(SScriptCallBack* p)
{
    std::cout << "entering spawnRobot" << std::endl;
    int stack = p->stackID;
    CStackArray args;
    args.buildFromStack(stack);
    if ( (args.getSize() > 0) && args.isString(0) )
    {
        ER->init_individual(args.getString(0));
        ind_initialised = true;
        std::cout << "individual initialised" << std::endl;
        CStackArray output;
        output.pushString("individual received");
        output.buildOntoStack(stack);
    }
    else
        simSetLastError(nullptr, "Not enough arguments or wrong arguments.");
    std::cout << "Exiting spawnRobot" << std::endl;
}

void sendRobotToClient(SScriptCallBack* p){
    int stack = p->stackID;
    CStackArray output;
    output.pushString(ER->get_currentInd()->to_string());
    output.buildOntoStack(stack);
    std::cout << "sent individual" << std::endl;
}

void checkConnection(SScriptCallBack* p){
    int stack = p->stackID;
    CStackArray args;
    args.buildFromStack(stack);
    if ( (args.getSize() > 0) && args.isString(0) )
    {
        CStackArray output;
        output.pushString("connected");
        output.buildOntoStack(stack);
    }
    else
        simSetLastError(nullptr, "Not enough arguments or wrong arguments.");
}

SIM_DLLEXPORT int simInit(SSimInit* info){ // This is called just once, at the start of CoppeliaSim.
    std::cout << "---------------------------" << std::endl
              << "STARTING WITH ARE FRAMEWORK" << std::endl
              << "---------------------------" << std::endl;
    simLib=loadSimLibrary(info->coppeliaSimLibPath);
    if (simLib==NULL)
    {
        simAddLog(info->pluginName,sim_verbosity_errors,"could not find all required functions in the CoppeliaSim library. Cannot start the plugin.");
        return(0); // Means error, CoppeliaSim will unload this plugin
    }
    if (getSimProcAddresses(simLib)==0)
    {
        simAddLog(info->pluginName,sim_verbosity_errors,"sorry, your CoppelisSim copy is somewhat old, CoppelisSim 4.0.0 rev1 or higher is required. Cannot start the plugin.");
        unloadSimLibrary(simLib);
        return(0); // Means error, CoppeliaSim will unload this plugin
    }



    are::settings::defaults::parameters->emplace("#evaluationOrder",std::make_shared<are::settings::Integer>(1)); //Default first in last out
    
    char* parameters_filepath = simGetStringParam(sim_stringparam_app_arg1);
    are::settings::ParametersMapPtr parameters = std::make_shared<are::settings::ParametersMap>(
                are::settings::loadParameters(parameters_filepath));
    simReleaseBuffer(parameters_filepath);
    bool verbose = are::settings::getParameter<are::settings::Boolean>(parameters,"#verbose").value;
    int seed = are::settings::getParameter<are::settings::Integer>(parameters,"#seed").value;

    ER = std::make_unique<are::sim::ER>();   // The class used to handle the EA

    if(verbose){
        std::cout << "Parameters Loaded" << std::endl;
        std::cout << "---------------------------------" << std::endl;
        std::cout << "loading experiment : " << are::settings::getParameter<are::settings::String>(parameters,"#expPluginName").value << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }

    ER->set_parameters(parameters);  // Initialize settings in the constructor
    if(seed < 0){//generate random seed
        std::random_device rd;
        seed = rd();
        are::settings::random::parameters->emplace("#seed",std::make_shared<are::settings::Integer>(seed));
    }
    are::misc::RandNum rn(seed);
    ER->set_randNum(std::make_shared<are::misc::RandNum>(rn));
    ER->initialize();

    simRegisterScriptCallbackFunction("getLogFolder",nullptr,getLogFolder);
    simRegisterScriptCallbackFunction("spawnRobot",nullptr,spawnRobot);
    simRegisterScriptCallbackFunction("initEnvironment",nullptr,initEnvironment);
    simRegisterScriptCallbackFunction("sendRobotToClient",nullptr,sendRobotToClient);
    simRegisterScriptCallbackFunction("checkConnection",nullptr,checkConnection);

    return 3;
}
SIM_DLLEXPORT void simMsg(SSimMsg* info){
    are::settings::ParametersMap param = (*ER->get_parameters());
    bool verbose = are::settings::getParameter<are::settings::Boolean>(param,"#verbose").value;
    int msg = info->msgId;
    // ABOUT TO START

    if (msg == sim_message_eventcallback_simulationabouttostart)
    {

        if (verbose)
            std::cout << "SIMULATION ABOUT TO START" << std::endl;
        // ER->initEnv();
        int engine[2] = {0,0};
        simSetIntArrayProperty(sim_handle_scene,"dynamicsEngine",engine,2);
        env_initialised = true;

    }
    else if (msg == sim_message_eventcallback_instancepass &&
            env_initialised && ind_initialised)
    {
        ER->handleSimulation(); // handling the simulation.
    }
    else if (msg == sim_message_eventcallback_instancepass &&
             !env_initialised && !ind_initialised)
    {
        simStopSimulation();
    }
    else if (msg == sim_message_eventcallback_simulationended)
    {
        ER->endOfSimulation();
        if (verbose)
            std::cout << "SIMULATION ENDED" << std::endl;
        env_initialised = false;
        ind_initialised = false;
    }
}
SIM_DLLEXPORT void simCleanup(){ // This is called just once, at the end of CoppeliaSim
    unloadSimLibrary(simLib); // release the library
}
