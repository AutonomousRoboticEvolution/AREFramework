#ifndef EA_HYPERNEAT_H
#define EA_HYPERNEAT_H

#include "ARE/EA.h"
#include "CPPNGenome.h"
#include "CPPNIndividual.h"
#include "EPuckMorphology.h"

namespace are {

class EA_HyperNEAT : public EA
{
public:
    EA_HyperNEAT() : EA(){}
    EA_HyperNEAT(const settings::ParametersMapPtr& param) : EA(param){}
    ~EA_HyperNEAT() override {}

    void init() override;
    void initPopulation(const NEAT::Parameters&);
    bool update() override;
    void epoch() override;

    float computeFitness(){}

    void setFitness(size_t indIndex, float fitness);

private:
    std::unique_ptr<NEAT::Population> neat_population;
    std::vector<float> currentFitnesses;
    size_t currentIndIndex;
    obs_t observations;
};

}

#endif //EA_HYPERNEAT_H
