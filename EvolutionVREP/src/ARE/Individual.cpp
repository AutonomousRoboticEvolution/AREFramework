#include <ARE/Individual.h>

using namespace are;

Individual::Individual(const Genome::Ptr &morph_gen,const Genome::Ptr &ctrl_gen) :
    morphGenome(morph_gen),ctrlGenome(ctrl_gen)
{
//    init();
}

Individual::~Individual(){
    morphGenome.reset();
    ctrlGenome.reset();
    morphology.reset();
    control.reset();
}
