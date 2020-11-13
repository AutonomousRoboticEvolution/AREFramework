#pragma once
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cmath>
#include <vector>
#include <iostream>
#include <memory>

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

}//are

#endif //ENVIRONMENT_H
