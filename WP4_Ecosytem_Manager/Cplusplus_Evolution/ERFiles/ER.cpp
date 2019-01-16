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

	if (atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 9) {
		simSet = RECALLBEST;
	}
	
	if (settings->evolutionType == settings->STEADY_STATE || settings->evolutionType == settings->GENERATIONAL && simSet != RECALLBEST) {
		// note that the environment pointer is empty in this class, the VREP relevant files assign a pointer to it, thus accessing it when
		// running the client will give a null pointer error.. Could I fix this?
		populations.push_back(unique_ptr<Population>(new Population(settings->populationSize, randNum, settings)));
		cout << "Population created" << endl;
		populations[0]->popIndNumbers = settings->indNumbers;
		indCounter = settings->individualCounter;
		for (int i = 0; i < populations[0]->populationGenomes.size(); i++) {
			populations[0]->populationGenomes[i]->mutationRate = settings->mutationRate;
		}
		for (int i = 0; i < populations[0]->populationGenomes.size(); i++) {
			populations[0]->populationGenomes[i]->maxAge = settings->maxAge;
		}

		if (settings->generation != 0) {
			populations[0]->loadPopulationGenomes(sceneNum);
		}

		if (settings->morphologyType == 0) {
			for (int i = 0; i < populations[0]->populationGenomes.size(); i++) {
				populations[0]->populationGenomes[i]->morphologyType = 2; // should be changed to 0 at some point
			}
		}
		if (settings->morphologyType == 1) {
			for (int i = 0; i < populations[0]->populationGenomes.size(); i++) {
				populations[0]->populationGenomes[i]->morphologyType = 1; // should be changed to 0 at some point
			}
		}
		if (settings->morphologyType == 3) {
			for (int i = 0; i < populations[0]->populationGenomes.size(); i++) {
				populations[0]->populationGenomes[i]->morphologyType = 3; // should be changed to 0 at some point
			}
		}
		if (settings->evolutionType == settings->GENERATIONAL) {
			populations[0]->nextGenFitness.resize(settings->populationSize);
		}
	}
	else if (settings->evolutionType == settings->EMBODIED_EVOLUTION) {
		populations.push_back(unique_ptr<Population>(new Population(1, randNum, settings)));
	}

	if (simSet == RECALLBEST) {
		populations.push_back(unique_ptr<Population>(new Population(1, randNum, settings)));
	}

	if (settings->generation != 0) {
		cout << "loading genomes" << endl;
		populations[0]->loadPopulationGenomes(sceneNum);
		indCounter = settings->individualCounter;
		indCounter++;
		generation = settings->generation;
		cout << "population loaded" << endl;
	}
	newGenerations = 0; /**/
}

void CER::initialize() {
	/* initialize the settings class; it will read a settings file or it 
	 * will use default parameters if it cannot read a settings file. 
	 * A random number class will also be created and all other files
	 * refer to this class. 
	 */

	//NEAT neat = new NEAT();
	settings = shared_ptr<Settings>(new Settings);
//	cout << "set seed to " << sceneNum << endl;
	shared_ptr<RandNum> newRandNum(new RandNum(settings->seed));
	randNum = newRandNum;
	newRandNum.reset();
	settings->repository = simGetStringParameter(sim_stringparam_app_arg3);
	settings->readSettings(sceneNum);
	initializeSimulation();
}


void CER::saveSettings() {
	cout << "saving" << endl;
	settings->generation = generation;
	settings->individualCounter = indCounter;
	vector<int> indNums;
	for (int i = 0; i < populations[0]->popIndNumbers.size(); i++) {
		indNums.push_back(populations[0]->popIndNumbers[i]); // must be set when saving
	}
	settings->indNumbers = indNums;

	int bestInd = 0;
	int bestIndividual = 0;
	float bestFitness = 0;
	for (int i = 0; i < populations[0]->populationFitness.size(); i++) {
		if (bestFitness < populations[0]->populationFitness[i]) {
			bestFitness = populations[0]->populationFitness[i];
			bestInd = i;
			bestIndividual = populations[0]->popIndNumbers[bestInd];
			cout << "Best individual has number " << populations[0]->popIndNumbers[bestInd] << endl;
		}
	}
	settings->bestIndividual = bestIndividual;
	
	settings->saveSettings(sceneNum);
	cout << "Settings saved" << endl;
}