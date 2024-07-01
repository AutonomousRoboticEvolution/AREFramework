#include "BOIndividual.hpp"

using namespace are;
namespace st = settings;

BOIndividual::BOIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
    sim::NN2Individual(morph_gen,ctrl_gen)
{

}

BOIndividual::BOIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen, const BOLearner::Ptr& l) :
    sim::NN2Individual(morph_gen,ctrl_gen)
{
    learner = l;
}

Individual::Ptr BOIndividual::clone()
{
    return std::make_shared<BOIndividual>(*this);
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

Eigen::VectorXd BOIndividual::descriptor()
{
    if(descriptor_type == FINAL_POSITION){
        double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
        Eigen::VectorXd desc(3);
        desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
        return desc;
    }else if(descriptor_type == VISITED_ZONES){
        Eigen::MatrixXd vz = visited_zones.cast<double>();
        Eigen::VectorXd desc(Eigen::Map<Eigen::VectorXd>(vz.data(),vz.cols()*vz.rows()));
        return desc;
    }else throw std::invalid_argument("NIPESIndvidual::descriptor(): descriptor type unknown");

}
