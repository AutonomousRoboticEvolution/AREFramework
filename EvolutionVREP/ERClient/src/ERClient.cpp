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
#include "ERClient/ClientEA.h"
#include <ctime>

using namespace are;
using namespace are::client;

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
    std::unique_ptr<ClientEA> client = std::unique_ptr<ClientEA>(new ClientEA);
	std::vector<std::string> arguments(argv + 1, argv + argc);
    client->properties.reset(new settings::Property);
    std::string parameters_file;

    if(arguments.size() <= 0)
    {
        std::cout << "usage : TODO";
        return 1;
    }

    parameters_file = arguments[0];
    settings::ParametersMap parameters = settings::loadParameters(parameters_file);
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(verbose)
    {
        std::cout << "arguments are: ";
        for (int i = 0; i < arguments.size(); i++) {
            std::cout << arguments[i] << ", ";
        }
        std::cout << std::endl;
    }

	// Just to handle settings
	if (arguments.size() > 1) {
		int run = atoi(arguments[1].c_str());


//		client->settings->sceneNum = run;
//		client->settings->seed = run;
        client->set_randNum(misc::RandNum(run));
		// client->randNum->setSeed(atoi(arguments[1].c_str()));
		srand(run);
	}
	else {
//		client->settings->seed = 0;
        client->set_randNum(misc::RandNum(0));
		// client->randNum->setSeed(0); // not initialized
		srand(0);
	}

	extApi_sleepMs(20000); // wait 20 seconds before connecting to ports, gives v-rep time to start up.
	//cout << "settings read" << endl;
	if (arguments.size() > 2) {
		std::cout << "client should connect to " << arguments[2].c_str() << " (-1) servers" << std::endl;
		int numberOfCores = atoi(arguments[2].c_str());
		if (!client->init(numberOfCores - 1)) {
			return -1; // could not properly connect to servers
		}
	}
	else {
		client->init(2);
	}

	// load or initialize EA
    int populationSize = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    int numberOfGeneration = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;
//    if (client->properties->generation != 0) {
//		std::cout << "Generation was not zero. Setting individual number to <generation>*<populationSize>" << std::endl;
//        client->properties->indCounter = (int)((client->properties->generation + 1) * populationSize); // could be read from settings instead
//		std::cout << "Loading Genomes" << std::endl;
//		client->ea->loadPopulationGenomes();
//		client->ea->selection(); // create the next gen, indCounter needs to be set first
//		client->settings->generation++; // increment because next gen will be evaluated
//	}
//	else {
		// client->ea->nextGenGenomes.resize(client->settings->populationSize);
		client->initGA();
        client->properties->indCounter = 0;

        if (verbose) {
			std::cout << "initialized EA " << std::endl;
		}
		//client->settings->generation += 1;
		//if (client->settings->indNumbers.size() < 1) {
		//	for (int i = 0; i < client->ea->nextGenGenomes.size(); i++) {
		//		client->settings->indNumbers.push_back(client->ea->nextGenGenomes[i]->individualNumber);
		//		client->settings->indFits.push_back(client->ea->nextGenGenomes[i]->fitness);
		//	}
		// }
//	}
    for (int i = client->properties->generation; i < numberOfGeneration; i++) {
	//	tStart = clock();
	//	client->ea->agePop(); // should be in update function of EA
		if (!client->evaluatePop()) {
            std::cout << "Something went wrong in the evaluation of the next generation. I am therefore quitting" << std::endl;
			break;
		}
//		client->ea->savePopFitness(i + 1, client->ea->popFitness);
		//client->settings->saveSettings(); // IS IN EVALUATEPOP
        if (verbose) {
            std::cout << "Just saved settings <right aftel evaluate pop>" << std::endl;
		}
//        if (client->properties->generation % client->properties->xGenerations == 0 && client->properties->generation!=0) {
//			std::cout << "Generation interval reached, quitting simulator. " << std::endl;
//			break;
//		}
        client->properties->indCounter += populationSize;
        client->properties->generation = i + 1;

        client->ea->epoch();

//		client->ea->selection(); // epochs in NEAT
//		if (client->settings->evolutionType == client->settings->EA_MULTINEAT) {
//		    auto *ea = dynamic_cast<EA_MultiNEAT *>(client->ea.get());
//		    std::ostringstream filename;
//		    filename << client->settings->repository << client->ea->neatSaveFile << client->settings->generation;
//			ea->savePopulation(filename.str());
//		}
	}

	extApi_sleepMs(5000);
	client->quitSimulators();
	extApi_sleepMs(8000);
	std::cout << "Client done, shutting down" << std::endl;
	return 0;
}

