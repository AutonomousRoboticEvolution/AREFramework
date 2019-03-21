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
	RECALLPOP
};

class CER
{
public:
	CER();
	virtual ~CER();

	/**
       @brief This method reads text files
	*/
	void split_line(string& line, string delim, list<string>& values);
	/// simulation setup detemining which genome to load for start
	simulationSetup simSet = DEFAULT_SETUP;
	///pointer to settting of EA
	shared_ptr<Settings> settings;  
	///pointer to EA
	unique_ptr<EA> ea;               
	///pointer to random number generator of EA
	shared_ptr<RandNum> randNum;

	/// ID of the genome
	int currentInd = 0;
	/// generation of ER
	int generation = 0;
  /// the same as generation?
	int newGenerations = 0;
	/// mark the initialization of ER
	bool _initialize = true;

    /**
	   @brief This method saves the genome and fitness 
	 */ 
	void saveSettings();
	/**
		 @brief initialize the settings class; it will read a settings file or it
		  will use default parameters if it cannot read a settings file.
		  A random number class will also be created and all other files
		  refer to this class.
	*/
	void initialize();  		   ///Is this method used?

	/**
	   @brief Empty method
	 */ 
	void initializeSimulation();   ///Is this method used?

	/**
		@brief This method load the genome 
	*/
	void loadIndividual(int individualNum, int sceneNum);
};

