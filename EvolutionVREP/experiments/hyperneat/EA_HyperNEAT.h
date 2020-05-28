#ifndef EA_HYPERNEAT_H
#define EA_HYPERNEAT_H

#include <chrono>
#include "ARE/EA.h"
#include "CPPNGenome.h"
#include "CPPNIndividual.h"
#include "mazeEnv.h"
#include "testEnv.h"

typedef std::chrono::high_resolution_clock hr_clock;

namespace are {

class EA_HyperNEAT : public EA
{
public:
    EA_HyperNEAT() : EA(){}
    EA_HyperNEAT(const settings::ParametersMapPtr& param) : EA(param){}
    ~EA_HyperNEAT() override {}

    void init() override;
    void initPopulation(const NEAT::Parameters&);
    void epoch() override;
    void init_next_pop();
    bool update(const Environment::Ptr &env) override;

    void setObjectives(size_t indIndex, const std::vector<double> &objectives);

private:
    std::unique_ptr<NEAT::Population> neat_population;
    int currentIndIndex;
};

}

#endif //EA_HYPERNEAT_H
