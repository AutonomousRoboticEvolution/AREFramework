#ifndef EA_HYPERNEAT_H
#define EA_HYPERNEAT_H

#include "ARE/EA.h"
#include "CPPNGenome.h"
#include "CPPNIndividual.h"
#include "EPuckMorphology.h"
#include "mazeEnv.h"
#include "testEnv.h"

namespace are {

class EA_HyperNEAT : public EA
{
public:
    EA_HyperNEAT() : EA(){}
    EA_HyperNEAT(const settings::ParametersMapPtr& param) : EA(param){}
    ~EA_HyperNEAT() override {}

    void init() override;
    void initPopulation(const NEAT::Parameters&);
    bool update(const Environment::Ptr&) override;
    void epoch() override;

    float computeFitness();

    void setFitness(size_t indIndex, float fitness);
    int getNumberOfEval() const {return currentFitnesses.size();}
    Eigen::VectorXd getLastObs() const {return partialObs.back();}
    Eigen::VectorXd getLastSpl() const {return partialSpl.back();}
    size_t get_currentIndIndex() const {return currentIndIndex;}

private:
    std::unique_ptr<NEAT::Population> neat_population;
    std::vector<float> currentFitnesses;
    size_t currentIndIndex;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    std::vector<Eigen::VectorXd> partialObs;
    std::vector<Eigen::VectorXd> partialSpl;
};

}

#endif //EA_HYPERNEAT_H
