#pragma once
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cmath>
#include <vector>
#include <iostream>
#include <memory>

#include "ARE/Genome.h"
#include "ARE/misc/RandNum.h"
#include "ARE/Settings.h"
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

    void from_string(const std::string& wp){
        std::stringstream sstr(wp);
        char delim[5];
        sstr >> position[0] >> delim[0] >> position[1] >> delim[1] >> position[2] >> delim[2]
             >> orientation[0] >> delim[3] >> orientation[1] >> delim[4] >> orientation[2];
    }

    bool is_nan(){
        return std::isnan(position[0]) || std::isnan(position[1]) || std::isnan(position[2]) ||
                std::isnan(orientation[0]) || std::isnan(orientation[1]) || std::isnan(orientation[2]);
    }

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & position;
        arch & orientation;
    }

};

class Environment{
public:

    typedef std::shared_ptr<Environment> Ptr;
    typedef std::shared_ptr<const Environment> ConstPtr;
    typedef Environment::Ptr (Factory)(const settings::ParametersMapPtr&);


    Environment(){}
    Environment(const Environment& env) :
        name(env.name),
        parameters(env.parameters),
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

    virtual void update_info(double time) = 0;

    virtual void print_info(){}

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const std::string &get_name(){return name;}
    void set_randNum(misc::RandNum::Ptr &rn){randNum = rn;}
    const std::vector<waypoint> &get_trajectory(){return trajectory;}
    const std::vector<double> &get_final_position(){return final_position;}

protected:
    std::string name;
    ///setting of the environment
    settings::ParametersMapPtr parameters;
    misc::RandNum::Ptr randNum;
    std::vector<waypoint> trajectory;
    std::vector<double> final_position;
};

class DummyEnv : public Environment
{
public:
    void init() override {}
    std::vector<double> fitnessFunction(const Individual::Ptr &ind) {return {0};}
    void update_info(double time) override {}
};

}//are

#endif //ENVIRONMENT_H
