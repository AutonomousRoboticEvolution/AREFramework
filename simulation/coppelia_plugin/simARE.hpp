#include <simLib/simExp.h>
#include <simLib/simTypes.h>
#include "simulatedER/ER.h"

/// an unique pointer to ER_VREP class
are::sim::ER::Ptr ER;
bool env_initialised = false;
bool ind_initialised = false;

//-- Callback functions

// void getLogFolder(SScriptCallBack* p);
// void spawnRobot(SScriptCallBack* p);
//--/

//-- CoppeliaSim functions
SIM_DLLEXPORT int simInit(SSimInit* info);
SIM_DLLEXPORT void simMsg(SSimMsg* info);
SIM_DLLEXPORT void simCleanup();

SIM_DLLEXPORT void simInit_ui(){} // called immediately after simInit
SIM_DLLEXPORT void simMsg_ui(SSimMsg_ui* info){}
SIM_DLLEXPORT void simCleanup_ui(){} // called immediately before simCleanup
//-/
