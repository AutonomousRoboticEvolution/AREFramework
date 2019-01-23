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
	void initializeSimulation(); // Loading Environment
	void initializeRobot(); // For connecting to actual robot.
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