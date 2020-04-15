#ifndef BOINDIVIDUAL_H
#define BOINDIVIDUAL_H

#include "ARE/Individual.h"
#include "EPuckMorphology.h"
#include "AREPuckMorphology.h"
#include "BOLearner.h"
#if defined(VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include "NNGenome.hpp"
#include "NNParamGenome.hpp"
#include "settings.hpp"

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
    void update_learner(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl, const Eigen::VectorXd &target);
    const Learner::Ptr &getLearner(){return learner;}

    void set_final_position(const std::vector<double> fp){final_position = fp;}
    const std::vector<double> get_final_position(){return final_position;}

    int genType = 2;

    std::string to_string();
    void from_string(const std::string&);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
    }


protected:
    void createController() override;
    void createMorphology() override;
    std::vector<double> final_position;
};

}//are

#endif //BOINDIVIDUAL_H
