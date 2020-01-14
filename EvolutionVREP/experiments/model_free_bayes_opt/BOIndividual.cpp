#include "BOIndividual.h"

using namespace are;

BOIndividual::BOIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
    Individual(morph_gen,ctrl_gen)
{

}

BOIndividual::BOIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen, const BOLearner::Ptr& l) :
    Individual(morph_gen,ctrl_gen)
{
    learner = l;
}

Individual::Ptr BOIndividual::clone()
{
    return std::make_shared<BOIndividual>(*this);
}

void BOIndividual::update(double delta_time)
{
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);

    std::vector<int> jointHandles =
            std::dynamic_pointer_cast<EPuckMorphology>(morphology)->get_jointHandles();

    assert(jointHandles.size() == outputs.size());

    for (size_t i = 0; i < outputs.size(); i++){
        sim::setJointVelocity(instance_type,
                              jointHandles[i],static_cast<float>(outputs[i]),properties->clientID);
    }
}

void BOIndividual::createMorphology()
{
    morphology.reset(new EPuckMorphology(parameters));
    morphology->set_properties(properties);
    morphology->createAtPosition(0,0,0);
}

void BOIndividual::createController()
{
    control.reset(new NNControl);
    control->set_properties(properties); 
    NEAT::NeuralNetwork nn;
    std::dynamic_pointer_cast<NEATGenome>(ctrlGenome)->neat_genome.BuildPhenotype(nn);
    std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
}

void BOIndividual::compute_model(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl){
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_samples(spl);
    std::dynamic_pointer_cast<BOLearner>(learner)->compute_model();
}

void BOIndividual::update_learner(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl)
{
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_samples(spl);
    std::dynamic_pointer_cast<BOLearner>(learner)->update_model();
    learner->update(control);
}

