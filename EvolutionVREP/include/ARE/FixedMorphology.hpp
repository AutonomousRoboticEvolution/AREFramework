#ifndef FIXED_MORPHOLOGY_HPP
#define FIXED_MORPHOLOGY_HPP

#include <cmath>
#include <boost/algorithm/string.hpp>
#include "ARE/Morphology.h"
#include "misc/coppelia_communication.hpp"

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
    FixedMorphology(const FixedMorphology& fm) : Morphology(fm),
        substrate(fm.substrate)
        {}

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
     * @brief set the position of the morphology with a random orientation
     */
    void setPosition(float,float,float);

    //GETTERS
    std::vector<int> get_jointHandles(){return jointHandles;}
    std::vector<int> get_wheelHandles(){return wheelHandles;}
    double get_energy_cost(){return energy_cost;}
    const NEAT::Substrate &get_substrate(){return substrate;}


private:
    NEAT::Substrate substrate;

};

}

#endif //FIXED_MORPHOLOGY_HPP
