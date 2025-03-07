// Copyright 2006-2016 Coppelia Robotics GmbH. All rights reserved. 
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
// This file was automatically created for V-REP release V3.3.2 on August 29th 2016

// Make sure to have the server side running in V-REP!
// Start the server from a child script with following command:
// simExtRemoteApiStart(portNumber) -- starts a remote API server service on the specified port

// ER files
#include "ERClient/clientER.h"
#include <ctime>
#include <boost/filesystem.hpp>
#include <random>
#include <execinfo.h>
#include <signal.h>
//#include <segvcatch.h>


using namespace are;


/**
 * @brief signal handler to generate a stacktrace after getting a segfault signal.
 * @param sig
 */
void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
int main(int argc, char* argv[])
{
    signal(SIGSEGV,handler);
    signal(SIGABRT,handler);
    signal(SIGILL,handler);
    signal(SIGFPE,handler);

    srand(time(NULL));

   // segvcatch::init_segv(&are::misc::handle_segv);


    if(argc != 4)
    {
        std::cout << "usage :" << std::endl;
        std::cout << "\targ1 - path to the parameter file" << std::endl;
        std::cout << "\targ2 - port of the first simulator instance" << std::endl;
        std::cout << "\targ3 - number of simulator instances" << std::endl;
        return 1;
    }

    std::unique_ptr<client::ER> client = std::make_unique<client::ER>();
    std::vector<std::string> arguments(argv + 1, argv + argc);
    std::string parameters_file = argv[1];
    int port = atoi(argv[2]);
    int nbInst = atoi(argv[3]);

    settings::defaults::parameters->emplace("#killWhenNotConnected",std::make_shared<const settings::Boolean>(false));
    settings::defaults::parameters->emplace("#shouldReopenConnections",std::make_shared<const settings::Boolean>(false));
    settings::defaults::parameters->emplace("#evaluationOrder",std::make_shared<const settings::Integer>(1)); //Default first in last out

    //Load the parameters
    settings::ParametersMapPtr parameters = std::make_shared<settings::ParametersMap>(
                settings::loadParameters(parameters_file));
    client->set_properties(std::make_shared<settings::Property>(settings::Property()));
    client->set_parameters(parameters);
    //-

    if(settings::getParameter<settings::Integer>(parameters,"#instanceType").value == settings::INSTANCE_REGULAR){
        std::cerr << "--- Launch Error ---"  << std::endl
                  << "Experiment launch as regular instance (sequential). This program should be launch in server instance (parallel)" << std::endl
                  << "Please set #instanceType parameter to 1 and relaunch the experiment." << std::endl
                  << "Aborting application." << std::endl;
        return 1;
    }

    int seed = settings::getParameter<settings::Integer>(parameters,"#seed").value;
    if(seed < 0){
        std::random_device rd;
        seed = rd();
        settings::random::parameters->emplace("#seed",std::make_shared<const settings::Integer>(seed));
    }
    misc::RandNum rn(seed);
    client->set_randNum(std::make_shared<misc::RandNum>(rn));

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(verbose)
    {
        std::cout << "arguments are: ";
        for (int i = 0; i < arguments.size(); i++) {
            std::cout << arguments[i] << ", ";
        }
        std::cout << std::endl;
    }

    if (!client->init(nbInst,port))
    {
        std::cerr << "Client unable to connect to the simulators instances !" << std::endl;
        return -1; // could not properly connect to servers
    }

    //Write parameters in the log folder.
    settings::saveParameters(are::Logging::log_folder + std::string("/parameters.csv"),parameters);

    // load or initialize EA
    client->get_properties()->indCounter = 0;
 
    if (verbose) {
        std::cout << "initialized EA " << std::endl;
    }

    //Main Loop
    while (client->execute()) usleep(5000);

    extApi_sleepMs(5000);
//client->quitSimulation();
    parameters.reset();
    client.reset();
    extApi_sleepMs(8000);
    std::cout << "Client done, shutting down" << std::endl;
    return 0;
}

