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
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if (verbose) {
        std::cout << "ER initialize" << std::endl;
    }
//    settings->indCounter = 0;
     if(instance_type == settings::INSTANCE_REGULAR)
    {
        client = true; // V-REP opens in client mode itself, this means there is no client-server relationship
        initializeSimulation(); // Initialize simulation
    }
    else if (instance_type == settings::INSTANCE_SERVER)    {
        client = false; // V-REP plugin will receive genomes from ea client
        initializeServer();
    }
}

/*!
 * Initializes ER as a server to accept genomes from client.
 *
 */
void ER::initializeServer()
{
    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;

    if(!load_fct_exp_plugin<Environment::Factory>
            (environmentFactory,exp_plugin_name,"environmentFactory"))
        exit(1);

//    if(!load_fct_exp_plugin<Genome::Factory>
//            (genomeFactory,settings->exp_plugin_name,"genomeFactory"))
//        exit(1);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,exp_plugin_name,"EAFactory"))
        exit(1);

    environment = environmentFactory(parameters);
    // initialize the environment
    environment->init();

    ea = EAFactory(randNum, parameters); // unique_ptr<EA>(new EA_VREP);

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

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;


    std::cout << "initialize simulation" << std::endl;
    if(!load_fct_exp_plugin<Environment::Factory>
            (environmentFactory,exp_plugin_name,"environmentFactory"))
        exit(1);

//    if(!load_fct_exp_plugin<Genome::Factory>
//            (genomeFactory,settings->exp_plugin_name,"genomeFactory"))
//        exit(1);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,exp_plugin_name,"EAFactory"))
        exit(1);

    std::cout << "external factories loaded !" << std::endl;

    environment = environmentFactory(parameters);
    ea = EAFactory(randNum, parameters);
    ea->init();
    environment->init();
}


/// When V-REP starts, this function is called. Depending on the settings, it initializes the properties of the
/// individual of the optimization strategy chosen.
void ER::startOfSimulation()
{
   if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
       std::cout << "Starting Simulation" << std::endl;

   currentInd = ea->getIndividual(currentIndIndex);
   currentInd->init();
}


void ER::handleSimulation()
{

    /* This function is called every simulation step. Note that the behavior of
    * the robot drastically changes when slowing down the simulation since this
    * function will be called more often. All simulated individuals will be
    * updated until the maximum simulation time, as specified in the environment
    * class, is reached.
    */
    if (settings::getParameter<settings::Integer>(parameters,"#instanceType").value
            == settings::INSTANCE_DEBUGGING) {
        simStopSimulation();
        return;
    }
    simulationTime += simGetSimulationTimeStep();
    currentInd->update(simulationTime);
    environment->updateEnv(simulationTime,currentInd->get_morphology());

    if (simGetSimulationTime() > environment->get_maxTime()) {
        simStopSimulation();
    }
}

void ER::endOfSimulation()
{

    int instanceType = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if(instanceType == settings::INSTANCE_SERVER){
        double fitness = environment->fitnessFunction(currentInd);
        // Environment independent fitness function:
        // float fitness = fit->fitnessFunction(currentMorphology);
//        float phenValue = currentGenome->morph->phenValue; // phenValue is used for morphological protection algorithm
        if(verbose)
            std::cout << "fitness = " << fitness << std::endl;
        simSetFloatSignal((simChar*) "fitness", fitness); // set fitness value to be received by client
//        simSetFloatSignal((simChar*) "phenValue", phenValue); // set phenValue, for morphological protection
        int signal[1] = { 2 };
        simSetIntegerSignal((simChar*) "simulationState", signal[0]);
//        if (settings->savePhenotype) {
//            currentGenome->fitness = fitness;
//            currentGenome->savePhenotype(currentGenome->individualNumber, settings->sceneNum);
//        }
    }else if(instanceType == settings::INSTANCE_REGULAR){


        if(currentIndIndex < ea->get_population().size())
        {
            double fitness = environment->fitnessFunction(currentInd);
            if(verbose)
                std::cout << "fitness = " << fitness << std::endl;
            ea->setFitness(currentIndIndex,fitness);
            currentIndIndex++;
        }
        else
        {
            ea->epoch();
            //        ea->savePopFitness(generation);
            //        generation++;
            //        saveSettings();
            currentIndIndex = 0;
        }

    }

}


//bool ER::loadIndividual(int individualNum)
//{
//    bool verbose = settings::cast<settings::Boolean>(parameters->at("#verbose"))->value;
//    if(verbose)
//        std::cout << "loading individual " << individualNum << std::endl;
//    currentInd = ea->getIndividual(individualNum);

//    //     currentGenome = genomeFactory(0, randNum, settings);
//    // try to load from signal
//    simInt signalLength = -1;
//    simInt signalLengthVerify = -1;
//    simChar* signal = simGetStringSignal("individualGenome", &signalLength);
//    simInt retValue = simGetIntegerSignal("individualGenomeLenght", &signalLengthVerify);

//    if (settings::cast<settings::Integer>(parameters->at("#verbose"))->value) {
//        if (signal != nullptr && signalLength != signalLengthVerify) {
//            std::cout << "genome received by signal, but length got corrupted, using file." << std::endl;
//            std::cout << signalLength << " != " << signalLengthVerify << std::endl;
//        }
//        std::cout << "loading genome " << individualNum << " from ";
//    }

//    bool load = false;
//    if (signal != nullptr && signalLength == signalLengthVerify)
//    {
//        // load from signal
//        if () {
//            std::cout << " signal." << std::endl;
//        }

//        const std::string individual((char*)signal, signalLength);
//        std::istringstream individualStream(individual);
//        load = currentGenome->loadGenome(individualGenomeStream, individualNum);
//    }
//    else
//    {
//        // load from file
//        if (settings::cast<settings::Integer>(parameters->at("#verbose"))->value) {
//            std::cout << " file." << std::endl;
//        }
//    }

//    if (signal != nullptr) {
//        simReleaseBuffer(signal);
//    }

//    //        currentGenome->set_individualNumber(individualNum);
//    std::cout << "loaded" << std::endl;
//    return load;
//}





//void ER::saveSettings()
//{
//    if (settings::cast<settings::Integer>(parameters->at("#verbose"))->value) {
//        std::cout << "Saving settings" << std::endl;
//	}
//	settings->generation = generation;
//	// TODO : remove redundancy
//	settings->individualCounter = settings->indCounter;
//    std::vector<int> indNums;
//    for (size_t i = 0; i < ea->get_population().size(); i++) {
//        indNums.push_back(ea->get_population()[i]->get_individual_id()); // must be set when saving
//	}
//	settings->indNumbers = indNums;

//	int bestInd = 0;
//	int bestIndividual = 0;
//	float bestFitness = 0;
//    for (size_t i = 0; i < ea->get_population().size(); i++) {
//        if (bestFitness < ea->get_population()[i]->getFitness()) {
//            bestFitness = ea->get_population()[i]->getFitness();
//			bestInd = i;
//            bestIndividual = ea->get_population()[bestInd]->get_individual_id();
//			if (settings->verbose) {
//                std::cout << "Best individual has number " << ea->get_population()[bestInd]->get_individual_id() << std::endl;
//			}
//		}
//	}
//	settings->bestIndividual = bestIndividual;
//	settings->saveSettings();
//	if (settings->verbose) {
//        std::cout << "Settings saved" << std::endl;
//	}
//}
