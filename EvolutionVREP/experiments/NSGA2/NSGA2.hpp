#ifndef NSGA2_HPP
#define NSGA2_HPP

#include "ARE/EA.h"
#include "ARE/Individual.h"

namespace are {

class NSGA2 : public EA
{
public:
    NSGA2() : EA(){}
    NSGA2(const settings::ParametersMapPtr& param) : EA(param){}
    ~NSGA2() {
        childrens.clear();
    }

    // base functions of EA
    void init();
    void selection();
    void replacement();
    void mutation();
    void crossover();

    Individual::Ptr tournament(const Individual::Ptr &,const Individual::Ptr &);
    void non_dominated_sort();

//    bool update(const Environment::Ptr&);

private:
    std::vector<Individual::Ptr> childrens;
    std::vector<Individual::Ptr> mixed_pop;
    NEAT::RNG rng;


};


}

#endif //NSGA2_HPP
