#ifndef FIXED_MORPHOLOGY_HPP
#define FIXED_MORPHOLOGY_HPP

#include "ARE/Morphology.h"
#include <cmath>
#if defined(VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

#include <boost/algorithm/string.hpp>

namespace are {

struct subtrates{
    static NEAT::Substrate are_puck;
    static NEAT::Substrate epuck;
};

/**
 * @brief Fixed morphology class. To use a robot from a model saved in .ttm file.
 */
class FixedMorphology : public Morphology
{
public:
    FixedMorphology(const settings::ParametersMapPtr &param) : Morphology(param){}
    Morphology::Ptr clone() const override
        {return std::make_shared<FixedMorphology>(*this);}

    /**
     * @brief create the robot morphology. The robot position will be at its default position.
     *      loadModel have to be called before this method !
     */
    void create() override;

    /**
     * @brief create the robot at position x, y, z by.
     *      loadModel have to be called before this method !
     * @param x
     * @param y
     * @param z
     */
    void createAtPosition(float x,float y,float z) override;

    /**
     * @brief method which read all the sensors of the morphology and return in the following order :
     *              - the proximity sensors values : distance to the nearest object detected
     *              - the passivIR sensors values : 0|1 value indicatings if an IR beacon is detected
     * @return sensor values
     */
    std::vector<double> update() override;

    /**
     * @brief load the model of the robot. The path of the model is written in the parameter file as #robotPath
     */
    void loadModel();

    /**
     * @brief set the neural network substrate of the robot. This is specific to hyperNEAT.
     * @param subtrate
     */
    void setSubstrate(NEAT::Substrate sub){
        substrate = sub;
    }


    /**
     * @brief read the values of the proximity sensors corresponding to the distance to the nearest object detected
     * @param sensor values (output)
     */
    void readProximitySensors(std::vector<double> &sensorValues);

    /**
     * @brief read the values of the passiv IR sensors corresponding to a binary value (0|1) indicating if an IR beacon is detected.
     * @param sensor values (output)
     */
    void readPassivIRSensors(std::vector<double> &sensorValues);


    /**
     * @brief set the position of the morphology.
     */
    void setPosition(float,float,float);
    std::vector<int> get_jointHandles(){return jointHandles;}

protected:
    void getObjectHandles() override;

private:
    std::vector<int> jointHandles;
    std::vector<int> proxHandles;
    std::vector<int> IRHandles;
    int cameraHandle;
};

}

#endif //FIXED_MORPHOLOGY_HPP
