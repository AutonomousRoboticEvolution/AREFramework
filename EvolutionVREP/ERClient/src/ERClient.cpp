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

    boost::filesystem::copy_file(parameters_file,are::Logging::log_folder);

    // load or initialize EA
    int populationSize = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    int numberOfGeneration = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;
    client->get_properties()->indCounter = 0;

    if (verbose) {
        std::cout << "initialized EA " << std::endl;
    }
    while (client->get_ea()->get_generation() < numberOfGeneration) {
        //	tStart = clock();
        //	client->ea->agePop(); // should be in update function of EA

        if (!client->execute()) {
            std::cerr << "Something went wrong in the evaluation of the next generation. I am therefore quitting" << std::endl;
            break;
        }
        //		client->ea->savePopFitness(i + 1, client->ea->popFitness);
        //client->settings->saveSettings(); // IS IN EVALUATEPOP
        //        if (verbose) {
        //            std::cout << "Just saved settings <right after evaluate pop>" << std::endl;
        //        }
        //        if (client->properties->generation % client->properties->xGenerations == 0 && client->properties->generation!=0) {
        //			std::cout << "Generation interval reached, quitting simulator. " << std::endl;
        //			break;
        //		}
        client->get_properties()->indCounter += populationSize;


        //		client->ea->selection(); // epochs in NEAT
        //		if (client->settings->evolutionType == client->settings->EA_MULTINEAT) {
        //		    auto *ea = dynamic_cast<EA_MultiNEAT *>(client->ea.get());
        //		    std::ostringstream filename;
        //		    filename << client->settings->repository << client->ea->neatSaveFile << client->settings->generation;
        //			ea->savePopulation(filename.str());
        //		}
    }


    extApi_sleepMs(5000);
    client->quitSimulation();
    parameters.reset();
    client.reset();
    extApi_sleepMs(8000);
    std::cout << "Client done, shutting down" << std::endl;
    return 0;
}

