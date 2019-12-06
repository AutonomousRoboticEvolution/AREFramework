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
    float timeStep = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;


    std::vector<double> inputs = morphology->update();

//    std::cout << "Inputs : ";
//    for(const double& prox : inputs)
//        std::cout << prox << " ; ";
//    std::cout << std::endl;

    previous_state.resize(inputs.size());
    for(int i = 0; i < inputs.size(); i++)
        previous_state(i) = inputs[i];

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

    inputs = morphology->update();


    current_state.resize(inputs.size());
    for(int i = 0; i < inputs.size(); i++)
        current_state(i) = inputs[i];

    int totalSteps = evalTime/timeStep;
    int nbrOfSteps = delta_time/timeStep;
    if(nbrOfSteps%(totalSteps/20) == 0)
        observations.push_back(get_observation());

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


void CPPNIndividual::update_learner(const std::vector<s_obs_t> &obs)
{
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    learner->update(control);
}
