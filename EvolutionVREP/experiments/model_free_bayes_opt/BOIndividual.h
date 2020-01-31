#ifndef BOINDIVIDUAL_H
#define BOINDIVIDUAL_H

#include "ARE/Individual.h"
#include "EPuckMorphology.h"
#include "BOLearner.h"
#include "v_repLib.h"

#include "NEATGenome.h"

namespace are {

class BOIndividual : public Individual
{
public :
    BOIndividual() : Individual(){}
    BOIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
    BOIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen, const BOLearner::Ptr& l);

    Individual::Ptr clone();

    void update(double delta_time);

    void compute_model(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl);
    void update_learner(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl);
    const Learner::Ptr &getLearner(){return learner;}

    void set_final_position(const std::vector<double> fp){final_position = fp;}
    const std::vector<double> get_final_position(){return final_position;}

protected:
    void createController() override;
    void createMorphology() override;
    std::vector<double> final_position;
};

}//are

#endif //BOINDIVIDUAL_H
