#ifndef LC_NSMS_H
#define LC_NSMS_H

#include "ARE/EA.h"
#include "ARE/CPPNGenome.h"
#include "ARE/Morphology_CPPNMatrix.h"
#include "CPPNIndividual.h"

#include "eigen3/Eigen/Core"

#include "ARE/learning/Novelty.hpp"

namespace are {

class LC_NSMS : public EA
{
public:
    LC_NSMS() : EA(){}
    LC_NSMS(const settings::ParametersMapPtr& param) : EA(param){}
    ~LC_NSMS() override {}

    void init() override;
    void initPopulation();
    void epoch() override;
    bool is_finish() override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;
    void init_next_pop() override;

private:
    std::unique_ptr<NEAT::Population> morph_population;

    NEAT::Parameters params;

    std::vector<Eigen::VectorXd> archive;

protected:
    NEAT::RNG rng;
    int currentIndIndex;

};

}

#endif //LC_NSMS_H
