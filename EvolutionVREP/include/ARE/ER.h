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

#include "EA.h"
//#include "ARE/EA_VREP.h"
//#include "ARE/EA_SteadyState.h"
#include "misc/RandNum.h"
#include "misc/utilities.h"


namespace are {

class ER
{
public:

    std::unique_ptr<ER> Ptr;
    std::unique_ptr<const ER> ConstPtr;

    ER();
    virtual ~ER();



    virtual void saveSettings();

    virtual void initialize();

    virtual void initializeSimulation();
    /// Indicates whether to start the simulation in server mode (received genome)
	bool startRun = true;

	// parameters
	/// Tracks the individual number (corresponding to genomes in the population)
	int currentInd = 0;
	/// generation counter
	int generation = 0;
	/// When recalling an evolutionary run, not only the generation is counted, but also the newGenerations. This allows the user to run evolution with generational intervals. 
	int newGenerations = 0; // deprecate?
	/// not used here
	void loadIndividual(int individualNum, int sceneNum);

protected:
    ///pointer to settting of EA
    Settings::Ptr settings;
    /// pointer to EA
    EA::Ptr ea;
    /// pointer to random number generator of EA
    misc::RandNum::Ptr randNum;

};

}//are

#endif //ER_H
