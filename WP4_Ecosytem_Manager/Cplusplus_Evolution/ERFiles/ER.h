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

	void split_line(string& line, string delim, list<string>& values);

	simulationSetup simSet = DEFAULT_SETUP;
	shared_ptr<Settings> settings;
	unique_ptr<EA> ea; // can be changed to another ea. Factory pattern?

	void saveSettings();
	void initialize();
	void initializeSimulation(); 

	// parameters
	int currentInd = 0;
	int generation = 0;
	int newGenerations = 0; 
	bool _initialize = true;

	void loadIndividual(int individualNum, int sceneNum);

	shared_ptr<RandNum> randNum;
};