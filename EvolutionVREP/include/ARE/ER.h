#pragma once
#ifndef ER_H
#define ER_H

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator> // for reading files and handling the acquired information
#include <fstream> // for storing arrays and using ofstream
#include <list>		// for reading files and handling the acquired information
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "ARE/EA.h"
#include "misc/RandNum.h"
#include "misc/utilities.h"
#include "ARE/Environment.h"

namespace are {

class ER
{
public:

    std::unique_ptr<ER> Ptr;
    std::unique_ptr<const ER> ConstPtr;

    typedef double (Fitness)(const Individual::Ptr& ind);

    ER();
    virtual ~ER();


    void initialize();

    void initializeSimulation();
    /// Initializes ER as a server to accept genomes from client.
    void initializeServer();

    void saveSettings();



	/// not used here
	void loadIndividual(int individualNum, int sceneNum);

    ///////////////////////
    /// V-REP Functions ///
    ///////////////////////
    /// Initializes ER as a server to accept genomes from client. If framework is server than just hold information for
    /// one genome. Else, initilizes the first population of individuals.
    void startOfSimulation();

    /// This function is called every simulation step. Note that the behavior of the robot drastically changes when
    /// slowing down the simulation since this function will be called more often. All simulated individuals will be
    /// updated until the maximum simulation time, as specified in the environment class, is reached.
    void handleSimulation();

    /// At the end of the simulation the fitness value of the simulated individual is retrieved and stored in the
    /// appropriate files.
    void endOfSimulation();

protected:
    ///pointer to settting of EA
    Settings::Ptr settings;
    /// pointer to EA
    EA::Ptr ea;
    /// pointer to random number generator of EA
    misc::RandNum::Ptr randNum;
    /// Pointer to the Environment class
    Environment::Ptr environment;
    ///Individual currently evaluated
    Individual::Ptr currentInd;

    ///used to create the environment
    std::function<Environment::Factory> environmentFactory;

    ///used to create the EA Algorithm
    std::function<EA::Factory> EAFactory;

    // parameters
    /// Tracks the individual number (corresponding to genomes in the population)
    int currentIndIndex = 0;
    /// generation counter
    int generation = 0;
    /// When recalling an evolutionary run, not only the generation is counted, but also the newGenerations. This allows the user to run evolution with generational intervals.
    int newGenerations = 0; // deprecate?
    /// Indicates whether to start the simulation in server mode (received genome)
    bool startRun = true;
    float simulationTime = 0;
};

}//are

#endif //ER_H
