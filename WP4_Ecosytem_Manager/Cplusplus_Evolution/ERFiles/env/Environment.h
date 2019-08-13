#pragma once
#include <vector>
#include <iostream>
#include "v_repLib.h"
#include <memory>
#include "../DefaultGenome.h"
#include "../module/ER_Module.h"
#include <cmath>

using namespace std; 

class Environment
{
public:
	Environment();
	~Environment();
	/**
	@brief Initialize the default environment scene and simulation time step size
	*/
	virtual void init() = 0;
	typedef shared_ptr<Morphology> MorphologyPointer;
	/**
	@brief Calculate the fitness value of the robot
	@param morph The pointer of the robot (morphology)
	*/
	virtual float fitnessFunction(MorphologyPointer morph) = 0;
	/**
	@brief update the info of the objects (e.g. robot) in the environments
	@param morph The pointer of the robot (morphology)
	*/
	virtual float updateEnv(MorphologyPointer morph) = 0;
	/**
	@brief Print debug information
	*/
	void print(); 
	/**
	@brief Load a specific scence
	*/
	void sceneLoader();
	///setting of the environment	
	shared_ptr<Settings> settings;
	///handle of the object in the environment	
	vector<int> envObjectHandles;
	///initial position of the object
	vector<float> initialPos; 
	///maximum simulation time
	float maxTime = 10.0; 
	int type = 0;
};

