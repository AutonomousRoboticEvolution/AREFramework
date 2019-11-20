#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "CPPNGenome.h"
#include "NNControl.h"
#include "EPuckMorphology.h"
#include "v_repLib.h"

namespace are {

class CPPNIndividual : public Individual
{
public :
    CPPNIndividual() : Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);

    Individual::Ptr clone();

    void update(double delta_time);


protected:
    void createController() override;
    void createMorphology() override;

};

}//are

#endif //CPPNINDIVIDUAL_H
