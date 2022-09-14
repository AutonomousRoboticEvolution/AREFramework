#ifndef MORPHOLOGY_HPP
#define MORPHOLOGY_HPP

#include <iostream>
#include <vector>
#include <memory>
#include "ARE/misc/RandNum.h"
#include "ARE/Settings.h"

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
        energy_cost(morph.energy_cost)
    {}
    virtual ~Morphology()
    {
	    parameters.reset();
        randomNum.reset();
    }

    virtual Morphology::Ptr clone() const = 0;
    /**
     * @brief method which read all the sensors of the morphology and return in the following order :
     *              - the proximity sensors values : distance to the nearest object detected
     *              - the passivIR sensors values : 0|1 value indicatings if an IR beacon is detected
     * @return sensor values
     */
    virtual std::vector<double> update() = 0;

    /**
     * @brief Sends command to the actuators of the robot with the following order : wheel commands and joints commands.
     */
    virtual void command(const std::vector<double>&) = 0;


    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    void set_randNum(misc::RandNum::Ptr& rn){randomNum = rn;}
    double get_energy_cost(){return energy_cost;}

protected:
    misc::RandNum::Ptr randomNum;
    settings::ParametersMapPtr parameters;
    double energy_cost = 0;
};

}//are

#endif //MORPHOLOGY_HPP

