#ifndef LinearCombination_H
#define LinearCombination_H

#include "ARE/EA.h"
#include "ARE/CPPNGenome.h"
#include "CPPNIndividual.h"
#include "ARE/Morphology_CPPNMatrix.h"

#include "eigen3/Eigen/Core"

#include "ARE/Novelty.hpp"

namespace are {

class LinearCombination : public EA
{
public:
    LinearCombination() : EA(){}
    LinearCombination(const settings::ParametersMapPtr& param) : EA(param){}
    ~LinearCombination() override {}

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

#endif //LinearCombination_H
