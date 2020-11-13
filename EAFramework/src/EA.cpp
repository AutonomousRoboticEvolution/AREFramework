#include "ARE/EA.h"

#include <memory>

using namespace are;

EA::EA(const settings::ParametersMapPtr& param){
    parameters = param;
}

EA::~EA()
{
    randomNum.reset();
    parameters.reset();
    for(auto& ind : population)
        ind.reset();
}


void EA::setSettings(const settings::ParametersMapPtr &param,const misc::RandNum::Ptr &rn)
{
    parameters = param;
	randomNum = rn;
}


void EA::epoch(){
    evaluation();
    selection();
}

void EA::init_next_pop(){
    replacement();
    crossover();
    mutation();
}


Individual::Ptr EA::getIndividual(size_t index) const
{
    return population[index];
}

