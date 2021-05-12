#pragma once
#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <iostream>
#include <vector>
#include <memory>
#include <multineat/Substrate.h>
//#include "ARE/ER_Module.h"
#include "misc/RandNum.h"
#include "ARE/Settings.h"
#include "misc/coppelia_communication.hpp"
//#include "ARE/Phenotype.h"
#include "eigen3/Eigen/Core"

namespace are {

/**
 * @brief The Morphology class
 */
class Morphology
{
public:

    typedef std::shared_ptr<Morphology> Ptr;
    typedef std::shared_ptr<const Morphology> ConstPtr;
    typedef Morphology::Ptr (Factory)(int,misc::RandNum::Ptr);

    Morphology(){}
    Morphology(const settings::ParametersMapPtr &param) : parameters(param){}
    Morphology(const Morphology& morph) :
        mainHandle(morph.mainHandle),
        jointHandles(morph.jointHandles),
        wheelHandles(morph.wheelHandles),
        proxHandles(morph.proxHandles),
        IRHandles(morph.IRHandles),
        cameraHandle(morph.cameraHandle),
        randomNum(morph.randomNum),
        energy_cost(morph.energy_cost)
    {}
    virtual ~Morphology()
    {
        parameters.reset();
        properties.reset();
        randomNum.reset();
    }

    virtual Morphology::Ptr clone() const = 0;

	/// This method creates the ns
	virtual void create() = 0;
	virtual void createAtPosition(float x, float y, float z) = 0;

    /**
     * @brief method which read all the sensors of the morphology and return in the following order :
     *              - the proximity sensors values : distance to the nearest object detected
     *              - the passivIR sensors values : 0|1 value indicatings if an IR beacon is detected
     * @return sensor values
     */
    virtual std::vector<double> update();

    /**
     * @brief Sends command to the actuators of the robot with the following order : wheel commands and joints commands.
     */
    virtual void command(const std::vector<double>&);

    //GETTERS & SETTERS
    virtual int getMainHandle(){return mainHandle;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}
    void set_client_id(int cid){client_id = cid;}
    int get_client_id(){return client_id;}
    void set_randNum(misc::RandNum::Ptr& rn){randomNum = rn;}
    void set_best_individual(int ind){best_individual = ind;}
    int get_best_individual(){return best_individual;}
    std::vector<int>& getJointHandles() {return jointHandles;}
    //TO be re-implemented
    NEAT::Substrate get_substrate() { return substrate;};
    bool stopSimulation;

protected:
    int mainHandle;

    // Handles used by the controller
    std::vector<int> jointHandles;
    std::vector<int> proxHandles;
    std::vector<int> IRHandles;
    std::vector<int> wheelHandles;
    int cameraHandle;

    int client_id;

    misc::RandNum::Ptr randomNum;
    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;
    double energy_cost = 0;

    NEAT::Substrate substrate;

    int best_individual;
};

}//are

#endif //MORPHOLOGY_H
