#include "BOCMAESIndividual.hpp"

using namespace are;

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
