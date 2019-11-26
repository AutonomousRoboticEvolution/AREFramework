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
#include "ARE/Phenotype.h"


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
        randomNum(morph.randomNum),
        substrate(morph.substrate)
    {}
    virtual ~Morphology()
    {
        parameters.reset();
        properties.reset();
        randomNum.reset();
    }

    virtual Morphology::Ptr clone() const = 0;

	/// This method creates the morphology
	virtual void create() = 0;
	virtual void createAtPosition(float x, float y, float z) = 0;

	/// This method updates the control of the morphology
    virtual std::vector<double> update() = 0;




    //GETTERS & SETTERS
    virtual int getMainHandle(){return mainHandle;}
    const NEAT::Substrate &get_substrate(){return substrate;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}

protected:


    /**
     * @brief Use this method to get the handles of the robot's components
     * @param parentHandle
     */
    virtual void getObjectHandles() = 0;

    int mainHandle;

    misc::RandNum::Ptr randomNum;
    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;

    NEAT::Substrate substrate;
};

}//are

#endif //MORPHOLOGY_H
