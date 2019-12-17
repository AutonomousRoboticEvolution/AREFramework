#ifndef EA_HYPERNEAT_H
#define EA_HYPERNEAT_H

#include "ARE/EA.h"
#include "BOIndividual.h"
#include "EPuckMorphology.h"
#include "NEATGenome.h"

namespace are {

class noEA : public EA
{
public:
    noEA() : EA(){}
    noEA(const settings::ParametersMapPtr& param) : EA(param){}
    ~noEA() override {}

    void init() override;
    bool update() override;
    void epoch() override;

    float computeFitness();

    void setFitness(size_t indIndex, float fitness);
    int getNumberOfEval() const {return currentFitnesses.size();}
    Eigen::VectorXd getLastObs() const {return observations.back();}
    Eigen::VectorXd getLastSpl() const {return samples.back();}


private:
    std::vector<float> currentFitnesses;
    size_t currentIndIndex;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
};

}

#endif //EA_HYPERNEAT_H
