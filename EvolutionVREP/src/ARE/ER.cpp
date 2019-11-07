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

/// Initialize the settings class; it will read a settings file or it will use default parameters if it cannot read a
/// settings file. A random number class will also be created and all other files refer to this class
void ER::initialize()
{

    if (settings->verbose) {
        std::cout << "ER initialize" << std::endl;
    }
    settings->indCounter = 0;
    if (settings->evolutionType != settings->EMBODIED_EVOLUTION
     && settings->instanceType == settings->INSTANCE_REGULAR)
    {
        settings->client = true; // V-REP opens in client mode itself, this means there is no client-server relationship
        initializeSimulation(); // Initialize simulation
    }
    else if (settings->evolutionType != settings->EMBODIED_EVOLUTION
          && settings->instanceType == settings->INSTANCE_SERVER
          && settings->startingCondition != settings->COND_LOAD_BEST)
    {
        settings->client = false; // V-REP plugin will receive genomes from ea client
        initializeServer();
    }
}

/*!
 * Initializes ER as a server to accept genomes from client.
 *
 */
void ER::initializeServer()
{

    if(!load_fct_exp_plugin<Environment::Factory>
            (environmentFactory,settings->exp_plugin_name,"environmentFactory"))
        exit(1);

//    if(!load_fct_exp_plugin<Genome::Factory>
//            (genomeFactory,settings->exp_plugin_name,"genomeFactory"))
//        exit(1);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,settings->exp_plugin_name,"EAFactory"))
        exit(1);

    environment = environmentFactory(settings);
    // initialize the environment
    environment->init();

    ea = EAFactory(randNum, settings); // unique_ptr<EA>(new EA_VREP);
    ea->randomNum = randNum;
    ea->init();
}

/// Initialize a genome factory to create genomes when the simulation is running
void ER::initializeSimulation()
{
//	genomeFactory = std::make_unique<GenomeFactoryVREP>();
//	genomeFactory->randomNum = randNum;
    // Environment factory is used to create the environment
//    EnvironmentFactory environmentFactory;
//	environment = environmentFactory.createNewEnvironment(settings);

    std::cout << "initialize simulation" << std::endl;
    if(!load_fct_exp_plugin<Environment::Factory>
            (environmentFactory,settings->exp_plugin_name,"environmentFactory"))
        exit(1);

//    if(!load_fct_exp_plugin<Genome::Factory>
//            (genomeFactory,settings->exp_plugin_name,"genomeFactory"))
//        exit(1);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,settings->exp_plugin_name,"EAFactory"))
        exit(1);

    std::cout << "external factories loaded !" << std::endl;

    environment = environmentFactory(settings);
    ea = EAFactory(randNum, settings);
    ea->randomNum = randNum;
    ea->init();
    environment->init();
}


/// When V-REP starts, this function is called. Depending on the settings, it initializes the properties of the
/// individual of the optimization strategy chosen.
void ER::startOfSimulation()
{
   currentInd = ea->getIndividual(currentIndIndex);
}


void ER::handleSimulation()
{

    /* This function is called every simulation step. Note that the behavior of
    * the robot drastically changes when slowing down the simulation since this
    * function will be called more often. All simulated individuals will be
    * updated until the maximum simulation time, as specified in the environment
    * class, is reached.
    */
    if (settings->instanceType == settings->INSTANCE_DEBUGGING) {
        simStopSimulation();
        return;
    }
    simulationTime += simGetSimulationTimeStep();
    environment->updateEnv(currentInd->get_morphology());
//    if (settings->evolutionType == settings->EA_MULTINEAT) {
//        ea->epoch();
//    } else {
////        currentGenome->update();
//    }
    if (simGetSimulationTime() > environment->maxTime) {
        simStopSimulation();
    }
}

void ER::endOfSimulation()
{
    if (settings->indCounter >= ea->populationGenomes.size())
    {
        double fitness = environment->fitnessFunction(currentInd);
        ea->setFitness(currentIndIndex,fitness);
        currentIndIndex++;
    }
    if (settings->indCounter % ea->nextGenGenomes.size() == 0 && settings->indCounter != 0)
    {
        ea->epoch();
        ea->savePopFitness(generation);
        generation++;
        saveSettings();
        newGenerations++;
    }

}


bool ER::loadIndividual(int individualNum)
{
    std::cout << "loading individual " << individualNum << ", sceneNum " << settings->sceneNum << std::endl;
   //     currentGenome = genomeFactory(0, randNum, settings);
        // try to load from signal
        simInt signalLength = -1;
        simInt signalLengthVerify = -1;
        simChar* signal = simGetStringSignal("individualGenome", &signalLength);
        simInt retValue = simGetIntegerSignal("individualGenomeLenght", &signalLengthVerify);

        if (settings->verbose) {
            if (signal != nullptr && signalLength != signalLengthVerify) {
                std::cout << "genome received by signal, but length got corrupted, using file." << std::endl;
                std::cout << signalLength << " != " << signalLengthVerify << std::endl;
            }
            std::cout << "loading genome " << individualNum << " from ";
        }

        bool load = false;
        if (signal != nullptr && signalLength == signalLengthVerify)
        {
            // load from signal
            if (settings->verbose) {
                std::cout << " signal." << std::endl;
            }

            const std::string individualGenome((char*)signal, signalLength);
            std::istringstream individualGenomeStream(individualGenome);
         //   load = currentGenome->loadGenome(individualGenomeStream, individualNum);
        }
        else
        {
            // load from file
            if (settings->verbose) {
                std::cout << " file." << std::endl;
            }
            if (settings->evolutionType == settings->EA_MULTINEAT) {
             //   ea->loadPopulationGenomes();
                // ea->createIndividual(individualNum); // this actually sets the NEAT genome
              //  individualToBeLoaded = individualNum;
                load = true;
            }
            else {
//               / load = currentGenome->loadGenome(individualNum, settings->sceneNum);
            }
        }

        if (signal != nullptr) {
            simReleaseBuffer(signal);
        }

//        currentGenome->set_individualNumber(individualNum);
        std::cout << "loaded" << std::endl;
        return load;
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
