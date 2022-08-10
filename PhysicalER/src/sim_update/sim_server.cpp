#include "physicalER/sim_update/sim_server.hpp"
//#include "luaFunctionData.h"
#include <iostream>
#include <fstream>
#include <memory>

#include "v_repLib.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <random>

namespace are_sett = are::settings;
namespace sim = are::sim;
namespace phy = are::phy;

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



VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer, int reservedInt)
{
    std::cout << "-------------------------------" << std::endl
              << "STARTING WITH ARE UPDATE SERVER" << std::endl
              << "-------------------------------" << std::endl;

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

    are_sett::defaults::parameters->emplace("#evaluationOrder",new are_sett::Integer(1)); //Default first in last out

    simChar* parameters_filepath = simGetStringParameter(sim_stringparam_app_arg1);
    are_sett::ParametersMapPtr parameters = std::make_shared<are_sett::ParametersMap>(
                are_sett::loadParameters(parameters_filepath));
    simReleaseBuffer(parameters_filepath);
    bool verbose = are_sett::getParameter<are_sett::Boolean>(parameters,"#verbose").value;
    int seed = are_sett::getParameter<are_sett::Integer>(parameters,"#seed").value;

//    // Construct classes
    ERVREP = std::make_unique<are::phy::update::ER>();   // The class used to handle the EA
    are_ctrl = std::make_unique<are::sim::AREControl>();

    if(verbose){
        std::cout << "Parameters Loaded" << std::endl;
        std::cout << "---------------------------------" << std::endl;
        std::cout << "loading experiment : " << are_sett::getParameter<are_sett::String>(parameters,"#expPluginName").value << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }

    ERVREP->set_parameters(parameters);  // Initialize settings in the constructor
    if(seed < 0){
        std::random_device rd;
        seed = rd();
        are_sett::random::parameters->emplace("#seed",new are_sett::Integer(seed));
    }
    are::misc::RandNum rn(seed);
    ERVREP->set_randNum(std::make_shared<are::misc::RandNum>(rn));
    ERVREP->initialize();

    //*/  Prepare our context and socket
    publisher.bind ("tcp://*:5555");
    reply.bind ("tcp://*:5556");
    /**/

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

    if(message == sim_message_eventcallback_modelloaded
            || message == sim_message_model_loaded
            || message == sim_message_eventcallback_sceneloaded
            || message == sim_message_scene_loaded
            || message == sim_message_eventcallback_modulehandleinsensingpart
           )
        return NULL;

    bool verbose = are_sett::getParameter<are_sett::Boolean>(ERVREP->get_parameters(),"#verbose").value;

    // ABOUT TO START
    if (message == sim_message_eventcallback_simulationabouttostart)
    {
        sim_started = true;
        if (verbose) {
            std::cout << "SIMULATION ABOUT TO START" << std::endl;
        }
        //        simStartSimulation();
        ERVREP->init_env();
        return NULL;


    }
    //Runing Simulation
    else if (message == sim_message_eventcallback_modulehandle)
    {

        float sim_time = simGetSimulationTime();
        std::dynamic_pointer_cast<are::sim::VirtualEnvironment>(ERVREP->get_environment())->updateEnv(sim_time,std::dynamic_pointer_cast<are::sim::Morphology>(are_ctrl->access_controller().get_morphology()));
        if(are_ctrl->exec(publisher,sim_time) == 0)
            simStopSimulation();
        return NULL;
    }
    // SIMULATION ENDED
    else if (message == sim_message_eventcallback_simulationended)
    {
        std::cout<<"finished running the controller"<<std::endl;
        are_ctrl->set_ready(false);
        are::Individual::Ptr ind;
        are_ctrl->access_controller().setObjectives(ERVREP->get_environment()->fitnessFunction(ind));
        are_ctrl->access_controller().set_trajectory(ERVREP->get_environment()->get_trajectory());
        std::cout << "fitness obtained ";
        for(const auto &obj: are_ctrl->access_controller().getObjectives())
            std::cout << obj << ";";
        std::cout << std::endl;
        are_ctrl->send_fitness(publisher);
        are_ctrl->send_trajectory(publisher);
        simInt length;
        simChar* log_folder = simGetStringSignal((simChar*) "log_folder", &length);
        if(log_folder != nullptr){
            are::Logging::log_folder = std::string(log_folder);
            are::Logging::log_folder.resize(length);
        }

        sim_started = false;
        return NULL;
    }

    if(!are_ctrl->is_ready()){

        bool is_cppn = are_sett::getParameter<are_sett::Boolean>(ERVREP->get_parameters(),"#isCPPNGenome").value;

        simStartSimulation();

        std::string str_ctrl, str_organs_list, str_param;

        std::cout << "Wait to receive messages" << std::endl;
        //receive parameters
        phy::receive_string(str_param,"parameters_received",reply,"pi ");
        std::cout<<"Parameters received:\n"<<str_param<<std::endl;
        are_sett::ParametersMapPtr parameters = std::make_shared<are_sett::ParametersMap>(are_sett::fromString(str_param));
        //receive organ addresses list
        phy::receive_string(str_organs_list,"organ_addresses_received",reply,"pi ");
        std::cout<<"Organs list received: \n"<<str_organs_list<<std::endl;
        are::Genome::Ptr morph_gen;
        if(is_cppn)
            morph_gen = std::make_shared<are::NN2CPPNGenome>(ERVREP->get_randNum(),parameters);
        else
            morph_gen = std::make_shared<are::ProtomatrixGenome>(ERVREP->get_randNum(),parameters);
        are::NNParamGenome::Ptr ctrl_gen(new are::NNParamGenome(ERVREP->get_randNum(),parameters));
        // this generates the neural network controller ind
        phy::receive_string(str_ctrl,"starting",reply,"pi ");
        std::string id = str_ctrl.substr(0,str_ctrl.find("\n"));
        str_ctrl.erase(0,str_ctrl.find("\n")+1);
        std::cout<<"NN Genome as string:\n"<<str_ctrl<<std::endl;
        ctrl_gen->from_string(str_ctrl);

        // retrieve morph_genome
        std::string log_repo = are_sett::getParameter<are_sett::String>(parameters,"#repository").value;
        std::string exp_name = are_sett::getParameter<are_sett::String>(parameters,"#experimentName").value;
        std::ifstream ifs(log_repo + exp_name + "/waiting_to_be_evaluated/morph_genome_" + id);
        if(!ifs){
            std::cerr << "Unable to open file :" << log_repo + exp_name + "/waiting_to_be_evaluated/morph_genome_" + id << std::endl;
            return NULL;
        }
        std::string str_morph{std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>()};
        morph_gen->from_string(str_morph);
        sim::AREIndividual ind(morph_gen,ctrl_gen);
        ind.set_parameters(parameters);
        ind.set_randNum(ERVREP->get_randNum());

    
        // run controller
        std::cout<<"init controller"<<std::endl;
        are_ctrl.reset();
        are_ctrl = std::make_unique<sim::AREControl>(ind, str_organs_list, parameters);
    }

    return NULL;
}

