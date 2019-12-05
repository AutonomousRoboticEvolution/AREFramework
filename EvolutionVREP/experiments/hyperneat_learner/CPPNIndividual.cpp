#include "CPPNIndividual.h"

using namespace are;

CPPNIndividual::CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
    Individual(morph_gen,ctrl_gen)
{

}

CPPNIndividual::CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen, const BOLearner::Ptr& l) :
    Individual(morph_gen,ctrl_gen)
{
    learner = l;
}



Individual::Ptr CPPNIndividual::clone()
{
    return std::make_shared<CPPNIndividual>(*this);
}

void CPPNIndividual::update(double delta_time)
{
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    std::vector<double> inputs = morphology->update();

//    std::cout << "Inputs : ";
//    for(const double& prox : inputs)
//        std::cout << prox << " ; ";
//    std::cout << std::endl;

    float pos[3];
    sim::getObjectPosition(instance_type,morphology->getMainHandle(),-1,pos); ///@todo for client/server mode;
    previous_state.resize(3);
    previous_state(0) = pos[0];
    previous_state(1) = pos[1];
    previous_state(2) = pos[2];

    std::vector<double> outputs = control->update(inputs);
    command.resize(2);
    command(0) = outputs[0];
    command(1) = outputs[1];

//    std::cout << "Outputs : ";
//    for(const double& o : outputs)
//        std::cout << o << " ; ";
//    std::cout << std::endl;

    std::vector<int> jointHandles =
            std::dynamic_pointer_cast<EPuckMorphology>(morphology)->get_jointHandles();

    assert(jointHandles.size() == outputs.size());

//    sim::pauseCommunication(instance_type,1,properties->clientID);
    for (size_t i = 0; i < outputs.size(); i++){
        sim::setJointVelocity(instance_type,
                              jointHandles[i],static_cast<float>(outputs[i]),properties->clientID);
    }
//    sim::pauseCommunication(instance_type,0,properties->clientID);

    sim::getObjectPosition(instance_type,morphology->getMainHandle(),-1,pos); ///@todo for client/server mode;
    current_state.resize(3);
    current_state(0) = pos[0];
    current_state(1) = pos[1];
    current_state(2) = pos[2];

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


void CPPNIndividual::update_learner(const obs_t &obs)
{
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    learner->update(control);
}
