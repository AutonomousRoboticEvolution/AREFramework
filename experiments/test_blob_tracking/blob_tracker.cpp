#include "blob_tracker.hpp"

using namespace are;

void BlobInd::init(){
    isEval = false;
}

void BlobInd::update(double delta_time){
//    isEval = true;
}

void BlobTracker::init(){
    Individual::Ptr ind(new BlobInd);
    ind->set_parameters(parameters);
    population.push_back(ind);
}
