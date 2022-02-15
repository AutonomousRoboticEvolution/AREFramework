#ifndef HYBRID_PARENT_POOL_HPP
#define HYBRID_PARENT_POOL_HPP

#include <memory>
#include "ARE/Genome.h"

namespace are {

typedef enum origin_t{
    SIMULATION = 0,
    REALITY = 1
} origin_t;

typedef struct genome_t{
    Genome::Ptr morph_genome;
    Genome::Ptr ctrl_genome;
    std::vector<double> objectives;
    int age;
    origin_t origin;
} genome_t;

struct DummyCrossover{
    std::vector<genome_t> operator ()(std::vector<genome_t> vr,std::vector<genome_t> rr){
        vr.insert(vr.end(),rr.begin(),rr.end());
        return vr;
    }
};

template<typename crossover_t>
class HybridParentPool{
public:
    typedef std::unique_ptr<HybridParentPool> Ptr;
    typedef std::unique_ptr<const HybridParentPool> ConstPtr;

    HybridParentPool(){}

    void add_robot();

    std::vector<genome_t> crossbreed(){return crossover(virtual_robot,real_robot);}

private:
    std::vector<genome_t> virtual_robot;
    std::vector<genome_t> real_robot;
    crossover_t crossover;

};

}

#endif //HYBRID_PARENT_POOL_HPP
