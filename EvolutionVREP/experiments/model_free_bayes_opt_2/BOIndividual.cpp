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
    std::string robot = settings::getParameter<settings::String>(parameters,"#robot").value;

    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);


    std::vector<int> jointHandles;
    if(robot == "EPuck"){
       jointHandles =
               std::dynamic_pointer_cast<EPuckMorphology>(morphology)->get_jointHandles();
    }else if(robot == "AREPuck"){
       jointHandles =
                std::dynamic_pointer_cast<AREPuckMorphology>(morphology)->get_jointHandles();
    }

    assert(jointHandles.size() == outputs.size());

    for (size_t i = 0; i < outputs.size(); i++)
        simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));

}

void BOIndividual::createMorphology()
{
    std::string robot = settings::getParameter<settings::String>(parameters,"#robot").value;

    if(robot == "EPuck"){
        morphology.reset(new EPuckMorphology(parameters));
        std::dynamic_pointer_cast<EPuckMorphology>(morphology)->loadModel();
    }
    else if(robot == "AREPuck"){
        morphology.reset(new AREPuckMorphology(parameters));
        std::dynamic_pointer_cast<AREPuckMorphology>(morphology)->loadModel();
    }

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);}

void BOIndividual::createController()
{
    control.reset(new NNControl);
    control->set_parameters(parameters);
    NEAT::NeuralNetwork nn;
    std::dynamic_pointer_cast<NEATGenome>(ctrlGenome)->neat_genome.BuildPhenotype(nn);
    std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
}

void BOIndividual::compute_model(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl){
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_samples(spl);
    std::dynamic_pointer_cast<BOLearner>(learner)->compute_model();
}

void BOIndividual::update_learner(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl, const Eigen::VectorXd &target)
{
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_samples(spl);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_target(target);

    std::dynamic_pointer_cast<BOLearner>(learner)->update_model();
    learner->update(control);
}

