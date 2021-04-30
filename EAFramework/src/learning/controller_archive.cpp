#include "ARE/learning/controller_archive.hpp"

using namespace are;

void ControllerArchive::init(int max_wheels, int max_joints, int max_sensors)
{
    archive.resize(max_wheels+1);
    for(auto& a_w : archive){
        a_w.resize(max_joints+1);
        for(auto& a_j: a_w){
            a_j.resize(max_sensors+1);
        }
    }

    for(auto& w : archive){
        for(auto& j : w){
            for(auto& s : j){
                s.first.reset(new NNParamGenome());
                s.second = 0;
            }
        }
    }
}

void ControllerArchive::reset_fitnesses(){
    for(auto& w : archive){
        for(auto& j : w){
            for(auto& s : j){
                s.second = 0;
            }
        }
    }
}

void ControllerArchive::update(const NNParamGenome::Ptr &genome, double fitness, int wheels, int joints, int sensors){
    if(archive[wheels][joints][sensors].second < fitness)
        archive[wheels][joints][sensors] = std::make_pair(genome,fitness);
}

