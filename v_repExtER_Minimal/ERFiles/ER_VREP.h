#pragma once

#include "ER.h"
#include "env/Environment.h"
#include "env/EnvironmentFactory.h"
#include "../include/v_repLib.h"

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
	unique_ptr<EA_VREP> ea; // can be changed to another ea. Factory pattern?

	void saveSettings();

};