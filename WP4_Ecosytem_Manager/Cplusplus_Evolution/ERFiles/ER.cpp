// bubbleRob plugin artefact:
// "
// Copyright 2006-2014 Dr. Marc Andreas Freese. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// -------------------------------------------------------------------
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// 
// You are free to use/modify/distribute this file for whatever purpose!
// -------------------------------------------------------------------	
//
// This file was automatically created for V-REP release V3.1.0 on January 20th 2014

//**************************************************
/* This file represents the main class of the ER plugin. The ER plugin is
* currently in a pre-alpha stage and still under development. 
* contact Frank at f.veenstra00@gmail.com for more information. 
*/
//**************************************************
// "

#include "ER.h"
//#include <afxwin.h> ;
//***************************************************************************************//

CER::CER(){
	// to initialize ER
}
CER::~CER(){

}

// for reading text files
void CER::split_line(string& line, string delim, list<string>& values)
{
	size_t pos = 0;
	while ((pos = line.find(delim, (pos + 1))) != string::npos) {
		string p = line.substr(0, pos);
		values.push_back(p);
		line = line.substr(pos + 1);
	}

	if (!line.empty()) {
		values.push_back(line);
	}
}

void CER::initializeSimulation() {

	// if (atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 9) {
	//	simSet = RECALLBEST;
	// }
}

void CER::loadIndividual(int individualNum, int sceneNum)
{
	cout << "ERROR:" << endl;
	cout << "	Trying to load an individual in a non-VREP instance, " << endl;
	cout << "	Make sure you're using ER_VREP to load robots. " << endl;

}

void CER::initialize() {
	/* initialize the settings class; it will read a settings file or it 
	 * will use default parameters if it cannot read a settings file. 
	 * A random number class will also be created and all other files
	 * refer to this class. 
	 */

	settings = shared_ptr<Settings>(new Settings);
	shared_ptr<RandNum> newRandNum(new RandNum(settings->seed));
	randNum = newRandNum;
	newRandNum.reset();
	settings->setRepository(simGetStringParameter(sim_stringparam_app_arg3));
	settings->readSettings();

	//TODO : Factory
	ea = unique_ptr<EA>(new EA_SteadyState); // default evolutionary algorithm
	ea->setSettings(settings, randNum);
	ea->init();

	initializeSimulation();
}


void CER::saveSettings() {
	if (settings->verbose) {
		cout << "Saving settings" << endl;
	}
	settings->generation = generation;
	// TODO : remove redundancy
	settings->individualCounter = settings->indCounter;
	vector<int> indNums;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
		indNums.push_back(ea->populationGenomes[i]->individualNumber); // must be set when saving
	}
	settings->indNumbers = indNums;

	int bestInd = 0;
	int bestIndividual = 0;
	float bestFitness = 0;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
		if (bestFitness < ea->populationGenomes[i]->fitness) {
			bestFitness = ea->populationGenomes[i]->fitness;
			bestInd = i;
			bestIndividual = ea->populationGenomes[bestInd]->individualNumber;
			if (settings->verbose) {
				cout << "Best individual has number " << ea->populationGenomes[bestInd]->individualNumber << endl;
			}
		}
	}
	settings->bestIndividual = bestIndividual;
	settings->saveSettings();
	if (settings->verbose) {
		cout << "Settings saved" << endl;
	}
}