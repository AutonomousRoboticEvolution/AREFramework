#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "CPPNGenome.h"
#include "EPuckMorphology.h"
#include "BOLearner.h"
#include "v_repLib.h"

namespace are {

class CPPNIndividual : public Individual
{
public :
    using s_obs_t = std::tuple<Eigen::VectorXd,Eigen::VectorXd,Eigen::VectorXd>; //single observation

    CPPNIndividual() : Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen, const BOLearner::Ptr& l);

    Individual::Ptr clone();

    void update(double delta_time);



    void update_learner(const std::vector<s_obs_t> &obs);

    std::vector<s_obs_t> get_observations(){return observations;}

protected:
    void createController() override;
    void createMorphology() override;

private:
    s_obs_t get_observation()
    {
        return std::make_tuple(previous_state,command,current_state);
    }

    Eigen::VectorXd previous_state;
    Eigen::VectorXd command;
    Eigen::VectorXd current_state;
    std::vector<s_obs_t> observations;

};

}//are

#endif //CPPNINDIVIDUAL_H
