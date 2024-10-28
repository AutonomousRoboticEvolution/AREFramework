#ifndef BODYPLANTESTING_H
#define BODYPLANTESTING_H

#include "ARE/EA.h"
#include "CPPNIndividual.h"
#include "eigen3/Eigen/Core"
#include "multineat/Population.h"
#include "ARE/learning/Novelty.hpp"


namespace are {


class BODYPLANTESTING : public NSGA2<CPPNIndividual>
{
public:
    struct novelty_params{
        static int k_value;
        static double novelty_thr;
        static double archive_adding_prob;
    };

    BODYPLANTESTING() : NSGA2<CPPNIndividual>(){}
    BODYPLANTESTING(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : NSGA2<CPPNIndividual>(rn, param){}
    ~BODYPLANTESTING() override {}

    void init() override;
    void initPopulation();
    void epoch() override;
    void init_next_pop() override;
    bool is_finish() override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;
    bool update(const Environment::Ptr&);


    //NEAT::Genome loadInd(short int genomeID); TODO replace NN2Genome
    std::vector<int> listInds();

private:
    std::vector<Eigen::VectorXd> archive;

protected:
    int currentIndIndex;
    int repetition =0;
    int highest_morph_id = 0;

};

}

#endif //BODYPLANTESTING_H
