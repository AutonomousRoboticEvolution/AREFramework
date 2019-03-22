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
		destination = "files";
	}
	client->settings->setRepository(destination);

	if (arguments.size() > 1) {
		int run = atoi(arguments[1].c_str());
		client->sceneNum = run; 
		client->settings->sceneNum = run; // sceneNum will be overridden...
		client->settings->seed = run; // seed will be overridden...
		client->settings->readSettings(); // essential, settings need to correspond with server settings
		// needs to be fixed at some point
		client->settings->sceneNum = run;
		client->settings->seed = run;
		client->randNum = shared_ptr<RandNum>(new RandNum(run));  //random number generator
		// client->randNum->setSeed(atoi(arguments[1].c_str()));
		srand(run);
	}
	else {
		client->settings->seed = 0;
		client->randNum = shared_ptr<RandNum>(new RandNum(0));
		// client->randNum->setSeed(0); // not initialized
		srand(0);
	}
	extApi_sleepMs(1000); // wait 5 seconds before connecting to ports
	//cout << "settings read" << endl;
	if (arguments.size() > 2) {   //if open more than 2 ports
		std::cout << "client should connect to " << arguments[2].c_str() << " (-1) servers" << std::endl;
		int numberOfNodes = atoi(arguments[2].c_str());
		client->init(numberOfNodes - 1);  //open a number of ports
	}
	else {
		client->init(2);
	}
	if (client->settings->generation != 0) {
		cout << "generation not set to 0????" << endl;
		client->ea->loadPopulationGenomes();  //load the current generation of genomes (both morph and control)
	}
	else {
		client->initGA();
		if (client->settings->verbose) {
			cout << "initialized EA " << endl;
		}
		client->evaluateInitialPop(); // initial generation
		if (client->settings->indNumbers.size() < 1) {
			client->settings->indNumbers.resize(client->ea->populationGenomes.size());
		}
		for (int i = 0; i < client->ea->populationGenomes.size(); i++) {
			client->settings->indNumbers[i] = client->ea->populationGenomes[i]->individualNumber;
		}
		//		client->settings->indFits = client->ea->popFitness;
	}
	

	int initialGen = client->settings->generation;
//	sysTime = clock();
//	for (int i = 0; i < 1000; i++) {
//		client->evaluateInitialPop();
//		saveLog(i);
//	}
	for (int i = initialGen; i < client->settings->maxGeneration; i++) {
	//	tStart = clock();
	//	client->ea->agePop(); // should be in update function of EA
		if (!client->evaluateNextGen()) {
			std::cout << "Something went wrong in the evaluation of the next generation. I am therefore quitting" << endl;
			break;
		}
//		client->ea->savePopFitness(i + 1, client->ea->popFitness);
		if (client->settings->indNumbers.size() < 1) {
			client->settings->indNumbers.resize(client->ea->populationGenomes.size());
		}
		for (int i = 0; i < client->ea->populationGenomes.size(); i++) {
			client->settings->indNumbers[i] = client->ea->populationGenomes[i]->individualNumber;
		}
		//		client->settings->indFits = client->ea->popFitness;
		client->settings->generation = i + 1;
		client->settings->saveSettings();
//		saveLog(i);
	}

	extApi_sleepMs(2000);
	client->quitSimulators();
	std::cout << "Client done, shutting down" << std::endl;
	return 0;
}

