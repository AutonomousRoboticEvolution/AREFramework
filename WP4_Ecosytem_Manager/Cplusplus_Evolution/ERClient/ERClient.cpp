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
#include "ClientEA.h"
#include <ctime>

clock_t tStart;
clock_t sysTime;

void saveLog(int counter) {
	ofstream logFile;
	logFile.open("files/timeLog.csv", ios::app);
	clock_t now = clock();
	//	double deltaSysTime = difftime((double) time(0), sysTime) ;
	int deltaSysTime = now - sysTime;
	logFile << "time after generation " << counter << " = ," << deltaSysTime  << "," << endl;
	sysTime = clock();
	logFile.close();
}

int main(int argc, char* argv[])
{
	unique_ptr<ClientEA> client = unique_ptr<ClientEA>(new ClientEA);
	std::vector<std::string> arguments(argv + 1, argv + argc);
	int amountGen = 0;
	client->settings = shared_ptr<Settings>(new Settings);
	string destination;
	if (arguments.size() > 0) {
		destination = arguments[0];
		cout << "arguments are: ";
		for (int i = 0; i < arguments.size(); i++) {
			cout << arguments[i] << ", ";
		}
		cout << endl;
	}
	else {
		destination = "files"; // default location
	}
	client->settings->setRepository(destination);

	// Just to handle settings
	if (arguments.size() > 1) {
		int run = atoi(arguments[1].c_str());
		client->settings->sceneNum = run; // sceneNum will be overridden...
		client->settings->seed = run; // sceneNum will be overridden...
		client->settings->readSettings(); // essential, settings need to correspond with server settings
		// needs to be fixed at some point
		client->settings->sceneNum = run;
		client->settings->seed = run;
		client->randNum = shared_ptr<RandNum>(new RandNum(run));
		// client->randNum->setSeed(atoi(arguments[1].c_str()));
		srand(run);
	}
	else {
		client->settings->seed = 0;
		client->randNum = shared_ptr<RandNum>(new RandNum(0));
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
	if (client->settings->generation != 0) {
		std::cout << "Generation was not zero. Setting individual number to <generation>*<populationSize>" << std::endl;
		client->settings->indCounter = (int)((client->settings->generation + 1) * client->settings->populationSize); // could be read from settings instead
		std::cout << "Loading Genomes" << std::endl;
		client->ea->loadPopulationGenomes();
		client->ea->selection(); // create the next gen, indCounter needs to be set first
		client->settings->generation++; // increment because next gen will be evaluated
	}
	else {
		// client->ea->nextGenGenomes.resize(client->settings->populationSize);
		client->initGA();
		client->settings->indCounter = 0;

		if (client->settings->verbose) {
			std::cout << "initialized EA " << std::endl;
		}
		//client->settings->generation += 1;
		//if (client->settings->indNumbers.size() < 1) {
		//	for (int i = 0; i < client->ea->nextGenGenomes.size(); i++) {
		//		client->settings->indNumbers.push_back(client->ea->nextGenGenomes[i]->individualNumber);
		//		client->settings->indFits.push_back(client->ea->nextGenGenomes[i]->fitness);
		//	}
		// }
	}
	for (int i = client->settings->generation; i < client->settings->maxGeneration; i++) {
	//	tStart = clock();
	//	client->ea->agePop(); // should be in update function of EA
		if (!client->evaluatePop()) {
			std::cout << "Something went wrong in the evaluation of the next generation. I am therefore quitting" << endl;
			break;
		}
//		client->ea->savePopFitness(i + 1, client->ea->popFitness);
		client->settings->saveSettings();
		if (client->settings->verbose) {

		}
		if (client->settings->generation % client->settings->xGenerations == 0 && client->settings->generation!=0) {
			std::cout << "Generation interval reached, quitting simulator. " << std::endl;
			break;
		}
		client->settings->indCounter += client->settings->populationSize;
		client->ea->selection();
		client->settings->generation = i + 1;
	}

	extApi_sleepMs(5000);
	client->quitSimulators();
	std::cout << "Client done, shutting down" << std::endl;
	return 0;
}

