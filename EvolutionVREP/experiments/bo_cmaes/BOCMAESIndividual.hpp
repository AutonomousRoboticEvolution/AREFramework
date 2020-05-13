#ifndef BOCMAESINDIVIDUAL_HPP
#define BOCMAESINDIVIDUAL_HPP

#include <ARE/Individual.h>
#include "FixedMorphology.hpp"
#include "NNGenome.hpp"
#include "NNParamGenome.hpp"
#include "BOLearner.h"

namespace are {



class BOCMAESIndividual : public Individual
{
public:
    BOCMAESIndividual() : Individual(){}
    BOCMAESIndividual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    BOCMAESIndividual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen,const Learner::Ptr l) :
        Individual(morph_gen,ctrl_gen){
        learner = l;
    }
    BOCMAESIndividual(const BOCMAESIndividual& ind) :
        Individual(ind),
        final_position(ind.final_position){}

    Individual::Ptr clone() override {
        return std::make_shared<BOCMAESIndividual>(*this);
    }

    void update(double delta_time) override;

    //specific to the current ARE arenas
    Eigen::VectorXd descriptor(){
        double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
        Eigen::VectorXd desc(3);
        desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
        return desc;
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


    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}

private:
    void createMorphology() override;
    void createController() override;

    std::vector<double> final_position;
};

}
#endif //CMAESINDIVIDUAL_HPP
