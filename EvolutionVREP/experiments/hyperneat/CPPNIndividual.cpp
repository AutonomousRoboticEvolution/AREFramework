#include "CPPNIndividual.h"

using namespace are;

CPPNIndividual::CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
    Individual(morph_gen,ctrl_gen)
{

}

Individual::Ptr CPPNIndividual::clone()
{
    return std::make_shared<CPPNIndividual>(*this);
}

void CPPNIndividual::update(double delta_time)
{
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    std::vector<double> inputs = morphology->update();

    std::cout << "Inputs : ";
    for(const double& prox : inputs)
        std::cout << prox << " ; ";
    std::cout << std::endl;

    std::vector<double> outputs = control->update(inputs);

    std::cout << "Outputs : ";
    for(const double& o : outputs)
        std::cout << o << " ; ";
    std::cout << std::endl;

    std::vector<int> jointHandles =
            std::dynamic_pointer_cast<EPuckMorphology>(morphology)->get_jointHandles();

    assert(jointHandles.size() == outputs.size());

    sim::pause(instance_type,properties->clientID);
    for (size_t i = 0; i < outputs.size(); i++)
        sim::setJointVelocity(instance_type,
                              jointHandles[i],static_cast<float>(outputs[i]),properties->clientID);
    sim::pause(instance_type,properties->clientID);
}

void CPPNIndividual::createMorphology()
{
    morphology.reset(new EPuckMorphology(parameters));
    morphology->set_properties(properties);
    morphology->createAtPosition(0,0,0);
}

void CPPNIndividual::createController()
{
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(ctrlGenome)->get_neat_genome();
    NEAT::Substrate subs = morphology->get_substrate();
    control.reset(new NNControl);
    control->set_properties(properties);
    NEAT::NeuralNetwork nn;
    gen.BuildHyperNEATPhenotype(nn,subs);
    std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
}


