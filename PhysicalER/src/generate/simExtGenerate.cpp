#include "physicalER/generate/simExtGenerate.h"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include <memory>
#include "simLib.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <random>

namespace are_set = are::settings;
namespace gen = are::phy::generate;

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
    std::cout << "----------------------" << std::endl
              << "STARTING ARE GENERATE" << std::endl
              << "----------------------" << std::endl;

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

    simChar* parameters_filepath = simGetStringParameter(sim_stringparam_app_arg1);

    ER = std::make_unique<gen::ER>();   // The class used to handle the EA
    are_set::ParametersMapPtr parameters = std::make_shared<are_set::ParametersMap>(
            are_set::loadParameters(parameters_filepath));
    simReleaseBuffer(parameters_filepath);
    ER->set_parameters(parameters);
    ER->initialize();
    ER->load_data();
    ER->generate();


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

    void* retVal=NULL;

    int errorModeSaved;
    simGetIntegerParameter(sim_intparam_error_report_mode, &errorModeSaved);
    simSetIntegerParameter(sim_intparam_error_report_mode, sim_api_errormessage_ignore);

    // ABOUT TO START
    if (message == sim_message_eventcallback_simulationabouttostart)
    {
        assert(simulationState == STARTING);
        simulationState = BUSY;
        ER->manufacturability_test();
    }
    //Runing Simulation
    else if (message == sim_message_eventcallback_modulehandle)
    {
        assert(simulationState == BUSY);
        simStopSimulation();
    }
    // SIMULATION ENDED
    else if (message == sim_message_eventcallback_simulationended)
    {
        assert(simulationState == BUSY);
        simulationState = CLEANUP;

        if(ER->is_finish()){
            ER->write_data();
            exit(0);
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
        simStartSimulation();
    }

    return retVal;
}
