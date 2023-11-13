#include "ARE/EA.h"

#include <memory>

using namespace are;

EA::EA(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param){
    parameters = param;
    randomNum = rn;
}

EA::~EA()
{
    randomNum.reset();
    parameters.reset();
    for(auto& ind : population)
        ind.reset();
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

void EA::fill_ind_to_eval(std::vector<int> &ind_to_eval){
    for(size_t i = 0; i < population.size(); i++)
        ind_to_eval.push_back(i);
}
