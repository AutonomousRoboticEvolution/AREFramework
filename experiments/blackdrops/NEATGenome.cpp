#include "NEATGenome.h"

using namespace are;

Phenotype::Ptr NEATGenome::develop(){
    NN nn;
    neat_genome.BuildPhenotype(nn.nn);
    return std::make_shared<NN>(nn);
}
