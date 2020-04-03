#ifndef BASIC_EA_H
#define BASIC_EA_H

#include "ARE/EA.h"
#include <memory>
#include "NNIndividual.hpp"
#include "mazeEnv.h"

namespace are {

class BasicEA : public EA
{
public:
    BasicEA() : EA(){}
    BasicEA(const settings::ParametersMapPtr& param) : EA(param){}
    ~BasicEA() {
        childrens.clear();
    }

    // base functions of EA
    void init();
    void selection();
    void replacement();
    void mutation();
    void crossover();

    bool update(const Environment::Ptr&);

    void setObjectives(size_t indIndex, const std::vector<double> &objectives);

private:
    std::vector<Individual::Ptr> childrens;
    NEAT::RNG rng;
    int currentIndIndex;

};

}//are

#endif //BASIC_EA_H
