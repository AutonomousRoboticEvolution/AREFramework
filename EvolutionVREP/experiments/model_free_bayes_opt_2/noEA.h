#ifndef NOEA_H
#define NOEA_H

#include <boost/filesystem.hpp>

#include "ARE/EA.h"
#include "BOIndividual.h"
#include "EPuckMorphology.h"
#include "NNGenome.hpp"
#include "NNParamGenome.hpp"
#include "testEnv.h"
#include "mazeEnv.h"
#include "settings.hpp"

namespace are {

class noEA : public EA
{
public:
    noEA() : EA(){}
    noEA(const settings::ParametersMapPtr& param) : EA(param){}
    ~noEA() override {}

    void init() override;
    void epoch();
    bool update(const Environment::Ptr &);

    bool is_finish();

    void setObjectives(size_t indIndex, const std::vector<double> &objectives);
    int getNumberOfEval() const {return currentFitnesses.size();}
    Eigen::VectorXd getLastObs() const {return observations.back();}
    Eigen::VectorXd getLastSpl() const {return samples.back();}

private:
    std::vector<float> currentFitnesses;
    size_t currentIndIndex;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    NEAT::RNG rng;
};

}

#endif //NOEA_H
