#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <memory>
#include "v_repLib.h"
#include "../DefaultGenome.h"
#include "../module/ER_Module.h"


class Environment
{
public:
    typedef std::shared_ptr<Environment> (Factory)(const std::shared_ptr<Settings>&);


	Environment();
	~Environment();
	/**
	@brief Initialize the default environment scene and simulation time step size
	*/
	virtual void init() = 0;
	typedef std::shared_ptr<Morphology> MorphologyPointer;
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
    std::shared_ptr<Settings> settings;
	///handle of the object in the environment	
    std::vector<int> envObjectHandles;
	///initial position of the object
    std::vector<float> initialPos;
	///maximum simulation time
	float maxTime = 0.0;
	int type = 0;
};
