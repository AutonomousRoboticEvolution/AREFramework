#pragma once
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cmath>
#include <vector>
#include <iostream>
#include <memory>
#if defined(VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include "ARE/Genome.h"
#include "ARE/Individual.h"


namespace are {

class Environment{
public:

    typedef std::shared_ptr<Environment> Ptr;
    typedef std::shared_ptr<const Environment> ConstPtr;
    typedef Environment::Ptr (Factory)(const settings::ParametersMapPtr&);


    Environment(){}
    Environment(const Environment& env) :
        parameters(env.parameters),
        name(env.name),
        randNum(env.randNum){}

    ~Environment(){}

    /**
     * @brief Initialize the default environment scene and simulation time step size
     */
    virtual void init() = 0;

    /**
     * @brief Calculate the fitness value of the robot
     * @param morph The pointer of the robot (morphology)
     */
    virtual std::vector<double> fitnessFunction(const Individual::Ptr &ind) = 0;

    virtual void update_info() = 0;

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const std::string &get_name(){return name;}
    void set_randNum(misc::RandNum::Ptr &rn){randNum = rn;}

protected:
    std::string name;
    ///setting of the environment
    settings::ParametersMapPtr parameters;
    misc::RandNum::Ptr randNum;
};

class VirtualEnvironment : public Environment
{
public:

    typedef std::shared_ptr<VirtualEnvironment> Ptr;
    typedef std::shared_ptr<const VirtualEnvironment> ConstPtr;
    typedef VirtualEnvironment::Ptr (Factory)(const settings::ParametersMapPtr&);


    VirtualEnvironment(){}
    VirtualEnvironment(const VirtualEnvironment& env) :
        parameters(env.parameters),
        envObjectHandles(env.envObjectHandles),
        initialPos(env.initialPos),
        maxTime(env.maxTime),
        name(env.name),
        randNum(env.randNum){}

    ~VirtualEnvironment(){}

    /**
     * @brief Initialize the default environment scene and simulation time step size
     */
    virtual void init();

    void sceneLoader();

    /**
     * @brief Calculate the fitness value of the robot
     * @param morph The pointer of the robot (morphology)
     */
    virtual std::vector<double> fitnessFunction(const Individual::Ptr &ind) = 0;

    virtual void update_info() override{};

    /**
     * @brief update the info of the objects (e.g. robot) in the environments
     * @param morph The pointer of the robot (morphology)
     */
    virtual float updateEnv(float simulationTime, const Morphology::Ptr &morph) = 0;


    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const std::vector<int> &get_envObjectHandles(){return envObjectHandles;}
    float get_maxTime(){return maxTime;}
    const std::string &get_name(){return name;}
    void set_randNum(misc::RandNum::Ptr &rn){randNum = rn;}
    const std::vector<double> &get_final_position(){return final_position;}
    const std::vector<waypoint> &get_trajectory(){return trajectory;}

protected:
    ///setting of the environment
    settings::ParametersMapPtr parameters;
    ///handle of the object in the environment
    std::vector<int> envObjectHandles;
    ///initial position of the object
    std::vector<float> initialPos;
    ///maximum simulation time
    float maxTime = 0.0;
    std::string name;
    misc::RandNum::Ptr randNum;
    std::vector<double> final_position;
    std::vector<waypoint> trajectory;
};

}//are

#endif //ENVIRONMENT_H
