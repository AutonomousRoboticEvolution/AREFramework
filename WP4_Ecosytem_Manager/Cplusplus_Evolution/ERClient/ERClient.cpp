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
#pragma once

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

	int amountGen = 0;

	client->settings = shared_ptr<Settings>(new Settings);
	string destination = argv[1];
	client->settings->repository = destination;
	client->settings->readSettings(0); // essential, settings need to correspond with server settings
	client->settings->seed = atoi(argv[2]);
	client->randNum->setSeed(atoi(argv[2]));
	srand(atoi(argv[2]));
	extApi_sleepMs(1000);
	cout << "settings read" << endl;
	client->init(6);
	if (client->settings->generation != 0) {
		client->pop->loadPopulationGenomes(0);
	}
	else {
		client->initGA();
		client->evaluateInitialPop(); // initial generation
		client->settings->indNumbers = client->pop->popIndNumbers;
		client->settings->indFits = client->pop->populationFitness;
	}
	

	int initialGen = client->settings->generation;
//	sysTime = clock();
//	for (int i = 0; i < 1000; i++) {
//		client->evaluateInitialPop();
//		saveLog(i);
//	}
	for (int i = initialGen; i < client->settings->maxGeneration; i++) {
	//	tStart = clock();
		client->pop->agePop();
		client->evaluateNextGen();
		client->pop->savePopFitness(i + 1, client->pop->populationFitness, client->sceneNum);
		client->settings->indNumbers = client->pop->popIndNumbers;
		client->settings->indFits = client->pop->populationFitness;
		client->settings->generation = i + 1;
		client->settings->saveSettings(client->sceneNum);
//		saveLog(i);
	}

	extApi_sleepMs(1000);
	client->quitSimulators();
	exit(0);

	return 0;
}

