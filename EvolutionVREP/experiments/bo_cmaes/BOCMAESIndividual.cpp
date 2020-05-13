#include "BOCMAESIndividual.hpp"

using namespace are;

void BOCMAESIndividual::createController(){
    control.reset(new NNControl);
    control->set_parameters(parameters);
    std::dynamic_pointer_cast<NNControl>(control)->set_randonNum(randNum);
    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

    NNGenome nn_constructor;
    nn_constructor.set_parameters(parameters);
    nn_constructor.init();
    NEAT::NeuralNetwork &nn = std::dynamic_pointer_cast<NNControl>(control)->nn;
    nn_constructor.buildPhenotype(nn);


    for(int i = 0; i < weights.size(); i++)
        nn.m_connections[i].m_weight = weights[i];
    for(int i = 0; i < bias.size(); i++)
        nn.m_neurons[i].m_bias = bias[i];
}

void BOCMAESIndividual::createMorphology(){
    morphology.reset(new FixedMorphology(parameters));
    std::dynamic_pointer_cast<FixedMorphology>(morphology)->loadModel();
    morphology->set_randNum(randNum);

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);
}

void BOCMAESIndividual::update(double delta_time){

    std::vector<double> inputs = morphology->update();

    std::vector<double> outputs = control->update(inputs);
    std::vector<int> jointHandles;
    jointHandles =
            std::dynamic_pointer_cast<FixedMorphology>(morphology)->get_jointHandles();

    assert(jointHandles.size() == outputs.size());

    for (size_t i = 0; i < outputs.size(); i++){
        simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));
    }
}

void BOCMAESIndividual::compute_model(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl){
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_samples(spl);
    std::dynamic_pointer_cast<BOLearner>(learner)->compute_model();
}

void BOCMAESIndividual::update_learner(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl, const Eigen::VectorXd &target)
{
    std::dynamic_pointer_cast<BOLearner>(learner)->set_observation(obs);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_samples(spl);
    std::dynamic_pointer_cast<BOLearner>(learner)->set_target(target);

    std::dynamic_pointer_cast<BOLearner>(learner)->update_model();
    std::dynamic_pointer_cast<BOLearner>(learner)->update(std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome));
}


std::string BOCMAESIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<BOCMAESIndividual>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void BOCMAESIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<BOCMAESIndividual>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;
}
