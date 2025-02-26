#ifndef EMPTYENV_HPP
#define EMPTYENV_HPP

#include <cmath>


#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"
#include "simulatedER/Morphology.h"

namespace are {

namespace sim{

class EmptyEnv : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<EmptyEnv> Ptr;
    typedef std::shared_ptr<const EmptyEnv> ConstPtr;

    EmptyEnv(){}

    ~EmptyEnv(){}
    void init() override{}

    std::vector<double> fitnessFunction(const Individual::Ptr &ind){}
    float updateEnv(float simulationTime, const Morphology::Ptr &morph){}

};

} //sim

} //are

#endif //EMPTYENV_HPP
