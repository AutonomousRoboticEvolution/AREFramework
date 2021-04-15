#include "dummyEA.hpp"


using namespace are;

void DummyInd::init(){
    isEval = false;
}

void DummyInd::update(double delta_time){
    isEval = true;
}

void DummyEA::init(){
    Individual::Ptr ind(new DummyInd);
    ind->set_parameters(parameters);
    population.push_back(ind);
}


