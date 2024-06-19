#include "BOIndividual.hpp"

using namespace are;
namespace st = settings;

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
    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->command(outputs);
}

void BOIndividual::createMorphology()
{
    morphology.reset(new sim::FixedMorphology(parameters));
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    morphology->set_randNum(randNum);

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,init_z);
}

void BOIndividual::createController()
{
    int nn_type = st::getParameter<st::Integer>(parameters,"#NNType").value;
    int nb_input = st::getParameter<st::Integer>(parameters,"#NbrInputNeurones").value;
    int nb_hidden = st::getParameter<st::Integer>(parameters,"#NbrHiddenNeurones").value;
    int nb_output = st::getParameter<st::Integer>(parameters,"#NbrOutputNeurones").value;

    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

    if(nn_type == st::nnType::FFNN){
        control.reset(new NN2Control<ffnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else if(nn_type == st::nnType::ELMAN){
        control.reset(new NN2Control<elman_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);

    }
    else if(nn_type == st::nnType::RNN){
        control.reset(new NN2Control<rnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else {
        std::cerr << "unknown type of neural network" << std::endl;
    }
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
    std::dynamic_pointer_cast<BOLearner>(learner)->update(std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome));
}

std::string BOIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<BOIndividual>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void BOIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<BOIndividual>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;
}

