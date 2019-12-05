#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "CPPNGenome.h"
#include "NNControl.h"
#include "EPuckMorphology.h"
#include "BOLearner.h"
#include "v_repLib.h"

namespace are {

class CPPNIndividual : public Individual
{
public :
    CPPNIndividual() : Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen, const BOLearner::Ptr& l);

    Individual::Ptr clone();

    void update(double delta_time);

    std::tuple<Eigen::VectorXd,Eigen::VectorXd,Eigen::VectorXd> get_observation()
    {
        return std::make_tuple(previous_state,command,current_state);
    }


    void update_learner(const obs_t &obs);

protected:
    void createController() override;
    void createMorphology() override;

private:
    Eigen::VectorXd previous_state;
    Eigen::VectorXd command;
    Eigen::VectorXd current_state;

};

}//are

#endif //CPPNINDIVIDUAL_H
