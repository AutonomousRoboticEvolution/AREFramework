#pragma once

#include "../RandNum.h"
#include <vector>
#include "DefaultGenome.h"
#include "Population.h"
//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>
#include "../include/v_repLib.h"
#include <iostream>
#include <thread>
#include <fstream> // for storing arrays and using ofstream
#include <cstdlib> // for converting values
#include <cstring>
#include <sstream>
#include <list>		// for reading files and handling the acquired information
#include <iterator> // for reading files and handling the acquired information
#include <string>
//#include <windows.h>
#include <vector>
#include <cassert>
#include <memory>
//#include "serial/Serial.h"

using namespace std;
//using namespace Parameters;

enum simulationSetup
{
	DEFAULT_SETUP,
	RECALLBEST,
	RECALLPOP
};

class CER
{
public:
	CER();
	virtual ~CER();

	simulationSetup simSet = DEFAULT_SETUP;
	shared_ptr<Settings> settings;
	void split_line(string& line, string delim, list<string>& values);
	void saveSettings();
	void initialize();
	void initializeSimulation(); 

	// parameters
	int indCounter = 0;
	int currentInd = 0;
	int generation = 0;
	int newGenerations = 0; 
	bool _initialize = true;
	int sceneNum = -1;
	int speciesLimit; // not used

	typedef shared_ptr<Population> PopulationPointer;
	vector<PopulationPointer> populations;
	shared_ptr<RandNum> randNum;
};