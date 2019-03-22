#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "v_repLib.h"
#include "v_repConst.h"
//#include "../ERFiles/Population.h"
#include "../ERFiles/SettingsERClient.h"
#include "../RandNum.h"
#include "../ERFiles/EA.h"
#include "../ERFiles/EA_SteadyState.h"
#include "../ERFiles/EA_Factory.h"

extern "C" {
	#include "v_repConst.h"
//	#include "../remoteApi/extApiPlatform.h"
//	#include "../remoteApi/extApi.h"
	#include "remoteApi/extApiPlatform.h"
	#include "remoteApi/extApi.h"
}

#define IPNUM = "127.0.0.1"

using namespace std;

struct IND {
	int nr;
	int sceneNr;
	float fitness;
};

///this class manages the EA
class ClientEA
{
public:
	vector<int> ports;
	simxChar* ipNum = "127.0.0.1"; // TO DO: should also be one argument
	vector<int> clientIDs;
	/// used to say which num of next genomes
	vector<int> portIndividual; 
	/// for sync between client and v-rep?
	vector<int> portState;
	/// actual number for loading genome
	vector<int> portIndividualNum; 

	shared_ptr<Settings> settings;
	shared_ptr<RandNum> randNum;
	vector<shared_ptr<IND>> queuedInds;
	//shared_ptr<Population> pop;
	shared_ptr<EA> ea;

	// functions
	void init(int amountPorts);
	/**
		@brief This method initialize a population of GA
	*/
	void initGA();
	void evaluateInitialPop();
	/**
		@brief This method creates the genomes for next generation
	*/
	void createNextGenGenomes();
	/**
		@brief This method evaluates next generation
	*/
	bool evaluateNextGen();
	void quitSimulators();

	int indCounter = 0;
	int sceneNum = 0;
	int loadingTrials = 1000;

	ClientEA();
	~ClientEA();
};

