#ifndef EA_HYPERNEAT_H
#define EA_HYPERNEAT_H

#include "ARE/EA.h"
#include "BOIndividual.h"
#include "EPuckMorphology.h"
#include "NEATGenome.h"
#include "testEnv.h"
#include "mazeEnv.h"

namespace are {

class noEA : public EA
{
public:
    noEA() : EA(){}
    noEA(const settings::ParametersMapPtr& param) : EA(param){}
    ~noEA() override {}

    void init() override;
    bool update(const Environment::Ptr&) override;
    void epoch() override;


    void setObjectives(size_t indIndex, const std::vector<double> &objectives);
    int getNumberOfEval() const {return currentFitnesses.size();}
    Eigen::VectorXd getLastObs() const {return observations.back();}
    Eigen::VectorXd getLastSpl() const {return samples.back();}

    const std::vector<double>& get_final_position() const {return final_position;}

private:
    std::vector<float> currentFitnesses;
    size_t currentIndIndex;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    NEAT::Parameters neat_params;
    std::vector<double> final_position;
};

}

#endif //EA_HYPERNEAT_H
