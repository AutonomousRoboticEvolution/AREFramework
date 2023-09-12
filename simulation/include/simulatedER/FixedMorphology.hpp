#ifndef FIXED_MORPHOLOGY_HPP
#define FIXED_MORPHOLOGY_HPP

#include <cmath>
#include <boost/algorithm/string.hpp>
#include "simulatedER/Morphology.h"

namespace are {

namespace sim {


/**
 * @brief Fixed morphology class. To use a robot from a model saved in .ttm file.
 */
class FixedMorphology : public Morphology
{
public:
    FixedMorphology(const settings::ParametersMapPtr &param) : Morphology(param){}
    FixedMorphology(const FixedMorphology& fm) : Morphology(fm){}

    are::Morphology::Ptr clone() const override
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
     * @brief set the position of the morphology with a random orientation
     */
    void setPosition(float,float,float);

    //GETTERS

    double get_energy_cost(){return energy_cost;}
};

}//sim

}//are

#endif //FIXED_MORPHOLOGY_HPP
