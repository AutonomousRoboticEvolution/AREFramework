#pragma once

#include "../RandNum.h"
#include <vector>
#include <stdlib.h>

#include <iostream>
#include <thread>
#include <fstream> // for storing arrays and using ofstream
#include <cstdlib> // for converting values
#include <cstring>
#include <sstream>
#include <list>		// for reading files and handling the acquired information
#include <iterator> // for reading files and handling the acquired information
#include <string>
#include <vector>
#include <cassert>
#include <memory>
#include "EA_VREP.h"
#include "EA_SteadyState.h"

using namespace std;

enum simulationSetup
{
	DEFAULT_SETUP,
	RECALLBEST,
	RECALLPOP,
	RECALLBESTFROMGENOME
};

class CER
{
public:
	CER();
	virtual ~CER();

	/**
       @brief This method splits lines
	*/
	void split_line(string& line, string delim, list<string>& values);
	/// simulation setup detemining which genome to load for start
	simulationSetup simSet = DEFAULT_SETUP;
	/// pointer to settting of EA
	shared_ptr<Settings> settings;  
	/// pointer to EA
	unique_ptr<EA> ea;               
	/// pointer to random number generator of EA
	shared_ptr<RandNum> randNum;

	void saveSettings();
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
