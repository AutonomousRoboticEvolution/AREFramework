#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "ARE/CPPNGenome.h"
#include "simulatedER/FixedMorphology.hpp"
#include "NNControl.h"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

namespace are {

class CPPNIndividual : public Individual
{
public :
    CPPNIndividual() : Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
    CPPNIndividual(const CPPNIndividual& ind) :
        Individual(ind), final_position(ind.final_position){}

    Individual::Ptr clone();

    void update(double delta_time);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
    }

    std::string to_string();
    void from_string(const std::string &str);

    void set_final_position(const std::vector<double> fp){final_position = fp;}
    const std::vector<double> get_final_position(){return final_position;}

protected:
    void createController() override;
    void createMorphology() override;

    std::vector<double> final_position;
};

}//are

#endif //CPPNINDIVIDUAL_H
