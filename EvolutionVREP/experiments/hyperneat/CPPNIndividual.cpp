#include "CPPNIndividual.h"

using namespace are;


void CPPNIndividual::createMorphology()
{
    morphology.reset(new EPuckMorphology);
    morphology->createAtPosition(0,0,0);
}

void CPPNIndividual::createControler()
{
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(ctrlGenome)->get_neat_genome();
    NEAT::Substrate subs = morphology->get_substrate();
    gen.BuildHyperNEATPhenotype(
                std::dynamic_pointer_cast<NNControl>(control)->nn,subs);
}


