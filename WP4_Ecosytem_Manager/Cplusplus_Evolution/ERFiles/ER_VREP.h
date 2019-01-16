#pragma once

#include "ER.h"
#include "env/Environment.h"
#include "env/EnvironmentFactory.h"
#include "morphology/Morphology.h"
#include "morphology/MorphologyFactoryVREP.h"
#include "PopulationVREP.h"

using namespace std;
//using namespace Parameters;

class ER_VREP : public CER
{
public:
	ER_VREP();
	virtual ~ER_VREP();

	void initializeSimulation();
	void initializeRobot();
	void initialize();
	void initializeServer(); 

	void startOfSimulation();
	void handleSimulation();
	void endOfSimulation();

	string mainHandleName;
	float simulationTime = 0;
	shared_ptr<Environment> environment;
	shared_ptr<Morphology> currentMorphology; 
	vector<unique_ptr<PopulationVREP>> populationsVREP;

	void saveSettings();

	// Whenever a VREP class creates a morphology,
	// this class should know about it so that it 
	// can give the morphology to the environment.
};