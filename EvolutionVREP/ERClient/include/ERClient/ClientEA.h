#pragma once
#ifndef CLIENTEA_H
#define CLIENTEA_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include "v_repLib.h"
#include "v_repConst.h"

#include "misc/RandNum.h"
#include "ARE/EA.h"
#include "ERClient/SlaveConnection.h"
#include "ARE/exp_plugin_loader.hpp"

extern "C" {
	#include "v_repConst.h"
//	#include "../remoteApi/extApiPlatform.h"
//	#include "../remoteApi/extApi.h"
	#include "remoteApi/extApiPlatform.h"
	#include "remoteApi/extApi.h"
}


namespace are {

namespace client {

class ClientEA
{
public:
	simxChar* ipNum = "127.0.0.1"; // TO DO: should also be one argument

	// vector<int> ports;
	// vector<int> clientIDs;
	// vector<int> portIndividual; // used to say which num of next genomes
	// vector<int> portState;
	// vector<int> portIndividualNum; // actual number for loading genome


//	vector<std::shared_ptr<IND>> queuedInds;
	// the queued individuals point to the genome array. Not actual number
	std::vector<int> queuedInds;
	//std::shared_ptr<Population> pop;
	std::shared_ptr<EA> ea;

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

    settings::Property::Ptr properties;

    //GETTERS & SETTERS
    const misc::RandNum::Ptr &get_randNum(){return randNum;}
    void set_randNum(const misc::RandNum &rn){randNum.reset(new misc::RandNum(rn));}
    void set_parameters(const settings::ParametersMap &param){parameters.reset(new settings::ParametersMap(param));}

private:
    settings::ParametersMapPtr parameters;
    misc::RandNum::Ptr randNum;

};

} //client

} //are

#endif //CLIENTEA_H
