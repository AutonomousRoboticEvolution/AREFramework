#ifndef BOCMAESINDIVIDUAL_HPP
#define BOCMAESINDIVIDUAL_HPP

#include "NN2Individual.hpp"
#include "FixedMorphology.hpp"
#include "NNGenome.hpp"
#include "NNParamGenome.hpp"
#include "BOLearner.h"

namespace are {



class BOCMAESIndividual : public NN2Individual
{
public:
    typedef std::shared_ptr<BOCMAESIndividual> Ptr;
    typedef std::shared_ptr<const BOCMAESIndividual> ConstPtr;

    BOCMAESIndividual() : NN2Individual(){}
    BOCMAESIndividual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        NN2Individual(morph_gen,ctrl_gen){}
    BOCMAESIndividual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen,const Learner::Ptr l) :
        NN2Individual(morph_gen,ctrl_gen){
        learner = l;
    }
    BOCMAESIndividual(const BOCMAESIndividual& ind) :
        NN2Individual(ind){}

    Individual::Ptr clone() override {
        return std::make_shared<BOCMAESIndividual>(*this);
    }




    void addObjective(double obj){objectives.push_back(obj);}

    void compute_model(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl);
    void update_learner(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl, const Eigen::VectorXd &target);
    const Learner::Ptr &getLearner(){return learner;}

    std::string to_string();
    void from_string(const std::string&);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
    }

};

}
#endif //CMAESINDIVIDUAL_HPP
