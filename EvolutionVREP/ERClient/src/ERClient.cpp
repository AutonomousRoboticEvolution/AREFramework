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

using namespace are;

clock_t tStart;
clock_t sysTime;

void saveLog(int counter) {
    std::ofstream logFile;
    logFile.open("files/timeLog.csv", std::ios::app);
    std::clock_t now = std::clock();
    //	double deltaSysTime = difftime((double) time(0), sysTime) ;
    int deltaSysTime = now - sysTime;
    logFile << "time after generation " << counter << " = ," << deltaSysTime  << "," << std::endl;
    sysTime = clock();
    logFile.close();
}

int main(int argc, char* argv[])
{

    srand(time(NULL));

    if(argc != 4)
    {
        std::cout << "usage :" << std::endl;
        std::cout << "\targ1 - path to the parameter file" << std::endl;
        std::cout << "\targ2 - port of the first simulator instance" << std::endl;
        std::cout << "\targ3 - number of simulator instances" << std::endl;
        return 1;
    }

    std::unique_ptr<client::ER> client(new client::ER);
    std::vector<std::string> arguments(argv + 1, argv + argc);
    std::string parameters_file = argv[1];
    int port = atoi(argv[2]);
    int nbInst = atoi(argv[3]);

    //Load the parameters
    settings::ParametersMapPtr parameters = std::make_shared<settings::ParametersMap>(
                settings::loadParameters(parameters_file));
    client->set_properties(std::make_shared<settings::Property>(settings::Property()));
    client->set_parameters(parameters);
    //-

    //todo take the seed
    int seed = settings::getParameter<settings::Integer>(parameters,"#seed").value;
    if(seed < 0)
        seed = rand();
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
    std::ofstream ofs(are::Logging::log_folder + std::string("/parameters.csv"));
    for(const auto &elt : *parameters)
    {
        if(elt.first != "#seed"){
            if(elt.second->name == "bool"){
                ofs << elt.first << ",bool," << settings::cast<settings::Boolean>(elt.second)->value << std::endl;
            }
            else if(elt.second->name == "int"){
                ofs << elt.first << ",int," << settings::cast<settings::Integer>(elt.second)->value << std::endl;
            }
            else if(elt.second->name == "float"){
                ofs << elt.first << ",float," << settings::cast<settings::Float>(elt.second)->value << std::endl;
            }
            else if(elt.second->name == "double"){
                ofs << elt.first << ",double," << settings::cast<settings::Double>(elt.second)->value << std::endl;
            }
            else if(elt.second->name == "string"){
                ofs << elt.first << ",string," << settings::cast<settings::String>(elt.second)->value << std::endl;
            }
        }
        else ofs << "#seed" << ",int," << seed << std::endl;
    }
    ofs.close();
 
    if (verbose) {
        std::cout << "initialized EA " << std::endl;
    }

    //Main Loop
    while (client->execute()) usleep(10000);

    extApi_sleepMs(5000);
    client->quitSimulation();
    parameters.reset();
    client.reset();
    extApi_sleepMs(8000);
    std::cout << "Client done, shutting down" << std::endl;
    return 0;
}

