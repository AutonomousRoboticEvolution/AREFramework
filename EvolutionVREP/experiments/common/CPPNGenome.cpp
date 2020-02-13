#include "CPPNGenome.h"

using namespace are;

Phenotype::Ptr CPPNGenome::develop(){
    CPPN cppn;
    neat_genome.BuildPhenotype(cppn.nn);
    return std::make_shared<CPPN>(cppn);
}
