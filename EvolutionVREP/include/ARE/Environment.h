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

struct waypoint{
    float position[3];
    float orientation[3];

    std::string to_string() const{
        std::stringstream sstr;
        sstr << position[0] << "," << position[1] << "," << position[2] << ";"
             << orientation[0] << "," << orientation[1] << "," << orientation[2];
        return sstr.str();
    }

    bool is_nan(){
        bool isNan = false;
        for(int i = 0; i < 3; i++)
            isNan = isNan || std::isnan(position[0]) || std::isnan(orientation[0]);
        return isNan;
    }

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & position;
        arch & orientation;
    }

};

class Environment
{
public:

    typedef std::shared_ptr<Environment> Ptr;
    typedef std::shared_ptr<const Environment> ConstPtr;
    typedef Environment::Ptr (Factory)(const settings::ParametersMapPtr&);


    Environment(){}
    Environment(const Environment& env) :
        parameters(env.parameters),
        properties(env.properties),
        envObjectHandles(env.envObjectHandles),
        initialPos(env.initialPos),
        maxTime(env.maxTime),
        type(env.type),
        name(env.name),
        randNum(env.randNum){}

    ~Environment(){}
    /**
        @brief Initialize the default environment scene and simulation time step size
        */
    virtual void init();

    /**
     * @brief Compute the objectives
     * @param an individual
     * @return a vector of objectives
     */
    virtual std::vector<double> fitnessFunction(const Individual::Ptr &ind) = 0;

    /**
     * @brief Update the information of the robot and the simulation
     * @param simulationTime
     * @param morphology
     * @return ?
     */
    virtual float updateEnv(float simulationTime, const Morphology::Ptr &morph) = 0;
    /**
        @brief Print debug information
        */
    void print();
    /**
        @brief Load a specific scence
        */
    void sceneLoader();

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const std::vector<int> &get_envObjectHandles(){return envObjectHandles;}
    float get_maxTime(){return maxTime;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}
    const std::string &get_name(){return name;}
    void set_randNum(misc::RandNum::Ptr &rn){randNum = rn;}
    const std::vector<double> &get_final_position(){return final_position;}
    const std::vector<waypoint> &get_trajectory(){return trajectory;}

protected:
    ///setting of the environment
    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;
    ///handle of the object in the environment
    std::vector<int> envObjectHandles;
    ///initial position of the object
    std::vector<float> initialPos;
    ///maximum simulation time
    float maxTime = 0.0;
    int type = 0;
    std::string name;
    misc::RandNum::Ptr randNum;
    std::vector<double> final_position;
    std::vector<waypoint> trajectory;

};

}//are

#endif //ENVIRONMENT_H
