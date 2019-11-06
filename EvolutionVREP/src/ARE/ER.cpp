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

#include "ARE/ER.h"

using namespace are;

ER::ER()
{
	// to initialize ER
}

ER::~ER()
{}

void ER::initializeSimulation()
{
	// if (atoi(simGetStringParameter(sim_stringparam_app_arg2)) == 9) {
	//	simSet = RECALLBEST;
	// }
}

void ER::loadIndividual(int individualNum, int sceneNum)
{
    std::cout << "ERROR:" << std::endl;
    std::cout << "	Trying to load an individual in a non-VREP instance, " << std::endl;
    std::cout << "	Make sure you're using ER_VREP to load robots. " << std::endl;

}

// TODO: Should this be deleted?
void ER::initialize()
{
    settings.reset(new Settings);
    misc::RandNum::Ptr newRandNum(new misc::RandNum(settings->seed));
	randNum = newRandNum;
	newRandNum.reset(); //destroy the pointer
	settings->setRepository(simGetStringParameter(sim_stringparam_app_arg3));  //pass the setting number from argument
	settings->readSettings();

	//TODO : Factory
//    ea = std::unique_ptr<EA>(new EA_SteadyState); // default evolutionary algorithm
//	ea->setSettings(settings, randNum);  //specify the setting and random number for EA
//	ea->init();

	initializeSimulation();  //empty function?
}

void ER::saveSettings()
{
	if (settings->verbose) {
        std::cout << "Saving settings" << std::endl;
	}
	settings->generation = generation;
	// TODO : remove redundancy
	settings->individualCounter = settings->indCounter;
    std::vector<int> indNums;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
        indNums.push_back(ea->populationGenomes[i]->get_individualNumber()); // must be set when saving
	}
	settings->indNumbers = indNums;

	int bestInd = 0;
	int bestIndividual = 0;
	float bestFitness = 0;
	for (int i = 0; i < ea->populationGenomes.size(); i++) {
        if (bestFitness < ea->populationGenomes[i]->get_fitness()) {
            bestFitness = ea->populationGenomes[i]->get_fitness();
			bestInd = i;
            bestIndividual = ea->populationGenomes[bestInd]->get_individualNumber();
			if (settings->verbose) {
                std::cout << "Best individual has number " << ea->populationGenomes[bestInd]->get_individualNumber() << std::endl;
			}
		}
	}
	settings->bestIndividual = bestIndividual;
	settings->saveSettings();
	if (settings->verbose) {
        std::cout << "Settings saved" << std::endl;
	}
}
