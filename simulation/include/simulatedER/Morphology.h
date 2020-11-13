#ifndef SIM_MORPHOLOGY_H
#define SIM_MORPHOLOGY_H

#include <iostream>
#include <vector>
#include <memory>
#include <multineat/Substrate.h>
#include "ARE/Morphology.hpp"
#include "ARE/misc/RandNum.h"
#include "ARE/Settings.h"
#include "simulatedER/coppelia_communication.hpp"

namespace are {

namespace sim {

/**
 * @brief The Morphology class
 */
class Morphology : public are::Morphology
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

    virtual are::Morphology::Ptr clone() const = 0;

    /// This method creates the morphology
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

    double get_energy_cost(){return energy_cost;}

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
};

}//sim

}//are

#endif //SIM_MORPHOLOGY_H
