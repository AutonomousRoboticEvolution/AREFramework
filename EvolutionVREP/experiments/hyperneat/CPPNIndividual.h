#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "ARE/CPPNGenome.h"
#include "ARE/NNControl.h"
#include "EPuckMorphology.h"

namespace are {

class CPPNIndividual : public Individual
{
public :
    CPPNIndividual() : Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}

    void update(double delta_time);

protected:
    void createControler() override;
    void createMorphology() override;
};

}//are

#endif //CPPNINDIVIDUAL_H
