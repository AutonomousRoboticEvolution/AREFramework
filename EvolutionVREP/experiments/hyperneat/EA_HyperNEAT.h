#ifndef EA_HYPERNEAT_H
#define EA_HYPERNEAT_H

#include "ARE/EA.h"
#include "ARE/CPPNGenome.h"
#include "CPPNIndividual.h"
#include "EPuckMorphology.h"

namespace are {

class EA_HyperNEAT : public EA
{
public:
    EA_HyperNEAT() : EA(){}
    EA_HyperNEAT(const Settings& st) : EA(st){}
    ~EA_HyperNEAT() override {}

    void init() override;
    void initPopulation(const NEAT::Parameters&);
    void epoch() override;

private:
    std::unique_ptr<NEAT::Population> neat_population;
};

}

#endif //EA_HYPERNEAT_H
