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
//	#include "../../remoteApi/extApiPlatform.h"
	#include "../../remoteApi/extApi.h" //../../remoteApi/extApi.h"
}

#define IPNUM = "127.0.0.1"

using namespace std;

struct IND {
	int nr;
	int sceneNr;
	float fitness;
};

class ClientEA
{
public:
	vector<int> ports;
	simxChar* ipNum = "127.0.0.1"; // TO DO: should also be one argument
	vector<int> clientIDs;
	vector<int> portIndividual;
	vector<int> portState;

	shared_ptr<Settings> settings;
	shared_ptr<RandNum> randNum;
	vector<shared_ptr<IND>> queuedInds;
	//shared_ptr<Population> pop;
	shared_ptr<EA> ea;

	// functions
	void init(int amountPorts);
	void initGA();
	void evaluateInitialPop();
	void createNextGenGenomes();
	void evaluateNextGen();
	void quitSimulators();

	int indCounter = 0;
	int sceneNum = 0;

	ClientEA();
	~ClientEA();
};

