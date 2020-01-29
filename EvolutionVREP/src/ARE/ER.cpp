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
namespace interproc = boost::interprocess;


/// Initialize the settings class; it will read a settings file or it will use default parameters if it cannot read a
/// settings file. A random number class will also be created and all other files refer to this class
void ER::initialize()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    if(instance_type == settings::INSTANCE_REGULAR){
        std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
        std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;

        Logging::create_log_folder(repository + std::string("/") + exp_name);
    }

    if (verbose) {
        std::cout << "ER initialize" << std::endl;
    }

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;


    if(!load_fct_exp_plugin<Environment::Factory>
            (environmentFactory,exp_plugin_name,"environmentFactory"))
        exit(1);

    environment = environmentFactory(parameters);
    if(!load_fct_exp_plugin<Logging::Factory>
            (loggingFactory,exp_plugin_name,"loggingFactory"))
        exit(1);

    loggingFactory(logs,parameters);


    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,exp_plugin_name,"EAFactory"))
        exit(1);
    ea = EAFactory(randNum, parameters);
    ea->init();
}


/// When V-REP starts, this function is called. Depending on the settings, it initializes the properties of the
/// individual of the optimization strategy chosen.
void ER::startOfSimulation()
{
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Simulation" << std::endl;

    simulationTime = 0;

    environment->init();

    currentInd = ea->getIndividual(currentIndIndex);
    currentInd->set_properties(properties);
    currentInd->init();
}

void ER::initIndividual(){
    simInt length;
    std::string mess(simGetStringSignal("currentInd",&length));
    if(length == 0){
        std::cerr << "No individual received" << std::endl;
        return;
    }
    currentInd = ea->getIndividual(0);
    currentInd->from_string(mess);
    currentInd->init();
    evalIsFinish = false;
}

void ER::handleSimulation()
{
    int instance_type =
            settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    /* This function is called every simulation step. Note that the behavior of
    * the robot drastically changes when slowing down the simulation since this
    * function will be called more often. All simulated individuals will be
    * updated until the maximum simulation time, as specified in the environment
    * class, is reached.
    */
    if (instance_type == settings::INSTANCE_DEBUGGING) {
        simStopSimulation();
        return;
    }

    simulationTime += simGetSimulationTimeStep();
//    if(instance_type == settings::INSTANCE_SERVER)
//        simSetFloatSignal("simulationTime",simulationTime);

    currentInd->update(simulationTime);
    environment->updateEnv(simulationTime,currentInd->get_morphology());

    if (simGetSimulationTime() >
            settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value) {
        simStopSimulation();
    }
}

void ER::endOfSimulation()
{

    int instanceType = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int nbrOfGen = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;

    if(verbose)
        std::cout << "individual " << currentIndIndex << " is evaluated" << std::endl;

    if(instanceType == settings::INSTANCE_REGULAR){
        if(currentIndIndex < ea->get_population().size())
        {
            double fitness = environment->fitnessFunction(currentInd);
            if(verbose)
                std::cout << "fitness = " << fitness << std::endl;
            ea->setFitness(currentIndIndex,fitness);
            if(ea->update())
              currentIndIndex++;
            saveLogs(false);
        }

        if(currentIndIndex >= ea->get_population().size())
        {
            saveLogs();
            ea->epoch();
            if(verbose)
                std::cout << "generation " << ea->get_generation() << " finished" << std::endl;
            ea->incr_generation();
            currentIndIndex = 0;

        }
        if(ea->get_generation() >= nbrOfGen){
            std::cout << "---------------------" << std::endl;
            std::cout << "Evolution is Finished" << std::endl;
            std::cout << "---------------------" << std::endl;
            exit(0);
        }
    }
    else if(instanceType == settings::INSTANCE_SERVER){
        double fitness = environment->fitnessFunction(currentInd);
        if(verbose)
            std::cout << "fitness = " << fitness << std::endl;
        ea->setFitness(currentIndIndex,fitness);
//        currentIndIndex++;

    }
}

void ER::saveLogs(bool endOfGen)
{
    for(const auto &log : logs){
        if(log->isEndOfGen() == endOfGen){
            log->saveLog(ea);
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
