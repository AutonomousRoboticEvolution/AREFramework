#pragma once

#include "ER.h"
#include "env/Environment.h"
#include "env/EnvironmentFactory.h"
#include "v_repLib.h"
#include "EA_Factory.h"

using namespace std;

class ER_VREP : public CER
{
public:
	ER_VREP();
	virtual ~ER_VREP();
	typedef shared_ptr<Morphology> MorphologyPointer;
	/**
		@brief Load environment
	*/
	void initializeSimulation();
	/**
		@brief Connect to actual robot
	*/
	void initializeRobot();
	/**
		 @brief initialize the settings class; it will read a settings file or it
	   will use default parameters if it cannot read a settings file.
	   A random number class will also be created and all other files
	   refer to this class.
	 */
	void initialize();
	void initializeServer();

	// V-REP
	void startOfSimulation();
	void handleSimulation();
	float fitnessFunction(MorphologyPointer morph);
	void endOfSimulation();

	float simulationTime = 0;
	shared_ptr<Environment> environment;

	// To keep track of the position of the robot
	string mainHandleName;
	shared_ptr<Morphology> currentMorphology;
	shared_ptr<Morphology> getMorphology(Genome* g);
	shared_ptr<EA> ea;
	shared_ptr<Genome> currentGenome;
	shared_ptr<GenomeFactoryVREP> genomeFactory;
	void saveSettings();

	// These functions used to be in newGenome
	bool loadIndividual(int individualNum);
	void loadBestIndividualGenome(int sceneNum);
};
