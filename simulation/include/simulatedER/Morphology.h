#ifndef SIM_MORPHOLOGY_H
#define SIM_MORPHOLOGY_H

#include <iostream>
#include <vector>
#include <memory>
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
    Morphology(const settings::ParametersMapPtr &param) : are::Morphology(param){}
    Morphology(const Morphology& morph) :
        mainHandle(morph.mainHandle),
        jointHandles(morph.jointHandles),
        wheelHandles(morph.wheelHandles),
        proxHandles(morph.proxHandles),
        IRHandles(morph.IRHandles),
        camera_handle(morph.camera_handle),
        morph_id(morph.morph_id)
    {}
    virtual ~Morphology()
    {
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

    std::vector<double> get_joints_positions();
    std::vector<double> get_wheels_positions();
    std::vector<double> get_position();

    /**
     * @brief Sends command to the actuators of the robot with the following order : wheel commands and joints commands.
     */
    virtual void command(const std::vector<double>&);


    //GETTERS & SETTERS
    virtual int getMainHandle(){return mainHandle;}

    void set_client_id(int cid){client_id = cid;}
    int get_client_id(){return client_id;}

    void set_morph_id(int id){morph_id = id;}
    int get_morph_id(){return morph_id;}

    std::vector<int> get_jointHandles(){return jointHandles;}
    std::vector<int> get_proxHandles(){return proxHandles;}
    std::vector<int> get_IRHandles(){return IRHandles;}
    std::vector<int> get_wheelHandles(){return wheelHandles;}

protected:
    int mainHandle;
    int morph_id;
    // Handles used by the controller
    std::vector<int> jointHandles;
    std::vector<int> proxHandles;
    std::vector<int> IRHandles;
    std::vector<int> wheelHandles;
    int camera_handle;

    int client_id;

};

}//sim

}//are

#endif //SIM_MORPHOLOGY_H
