#pragma once

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

#include "EA_VREP.h"
#include "EA_SteadyState.h"
#include "../RandNum.h"


class CER
{
public:
	CER();
	virtual ~CER();

	/**
       @brief This method splits lines
	*/
	void split_line(std::string& line, std::string delim, std::list<std::string>& values);

	///pointer to settting of EA
	std::shared_ptr<Settings> settings;
	/// pointer to EA
	std::unique_ptr<EA> ea;
	/// pointer to random number generator of EA
	std::shared_ptr<RandNum> randNum;

	void saveSettings();
	void initialize();
	void initializeSimulation();
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

};
