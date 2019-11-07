#include "CPPNIndividual.h"

using namespace are;

void CPPNIndividual::update(double delta_time)
{
    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);
    std::vector<int> jointHandles = morphology->get_jointHandles();

    assert(jointHandles.size() == outputs.size());

    for (size_t i = 0; i < outputs.size(); i++)
        simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));
}

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


