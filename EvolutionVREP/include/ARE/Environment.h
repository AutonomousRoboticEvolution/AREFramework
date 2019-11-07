#pragma once
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cmath>
#include <vector>
#include <iostream>
#include <memory>
#include "v_repLib.h"
#include "ARE/Genome.h"

#include "ARE/Individual.h"


namespace are {

class Environment
{
public:

    typedef std::shared_ptr<Environment> Ptr;
    typedef std::shared_ptr<const Environment> ConstPtr;
    typedef Environment::Ptr (Factory)(const Settings::Ptr&);


	Environment();
	~Environment();
	/**
	@brief Initialize the default environment scene and simulation time step size
	*/
    virtual void init();
	/**
	@brief Calculate the fitness value of the robot
	@param morph The pointer of the robot (morphology)
	*/
    virtual double fitnessFunction(const Individual::Ptr &ind) = 0;
	/**
	@brief update the info of the objects (e.g. robot) in the environments
	@param morph The pointer of the robot (morphology)
	*/
    virtual float updateEnv(const Morphology::Ptr &morph) = 0;
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

}//are

#endif //ENVIRONMENT_H
