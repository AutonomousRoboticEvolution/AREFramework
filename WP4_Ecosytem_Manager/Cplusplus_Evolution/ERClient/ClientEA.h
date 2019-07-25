#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <memory>

#include "v_repLib.h"
#include "v_repConst.h"
//#include "../ERFiles/Population.h"
#include "../ERFiles/SettingsERClient.h"
#include "../RandNum.h"
#include "../ERFiles/EA.h"
#include "../ERFiles/EA_SteadyState.h"
#include "../ERFiles/EA_Factory.h"
#include "SlaveConnection.h"

extern "C" {
	#include "v_repConst.h"
//	#include "../remoteApi/extApiPlatform.h"
//	#include "../remoteApi/extApi.h"
	#include "remoteApi/extApiPlatform.h"
	#include "remoteApi/extApi.h"
}

#define IPNUM = "127.0.0.1"

using namespace std;



class ClientEA
{
public:
	simxChar* ipNum = "127.0.0.1"; // TO DO: should also be one argument

	// vector<int> ports;
	// vector<int> clientIDs;
	// vector<int> portIndividual; // used to say which num of next genomes
	// vector<int> portState;
	// vector<int> portIndividualNum; // actual number for loading genome

	shared_ptr<Settings> settings;
	shared_ptr<RandNum> randNum;
//	vector<shared_ptr<IND>> queuedInds;
	// the queued individuals point to the genome array. Not actual number
	vector<int> queuedInds;
	//shared_ptr<Population> pop;
	shared_ptr<EA> ea;

	// functions
	bool init(int amountPorts, int startPort=104000);
	void initGA();
	void quitSimulators();

	int finishConnections();
	int openConnections();
	int reopenConnections();

	bool confirmConnections();

	bool evaluatePop(); // replaces evaluate initial and evaluate next

	int loadingTrials = 1000;
	
	struct ServerInstance {
		int portNum; // number assigned
		int port; // actual port
		int individual; // current individual in newGenomes
		int individualNum; // actual individual number
		int state; // signify what state the connection is in (8 = running, 9 = could not load, 0 = )
	};

	std::vector<std::unique_ptr<SlaveConnection>> serverInstances;
	
	ClientEA();
	~ClientEA();
};

