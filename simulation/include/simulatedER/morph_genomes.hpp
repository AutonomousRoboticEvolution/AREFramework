#ifndef MORPH_GENOME_HPP
#define MORPH_GENOME_HPP

#include "simulatedER/nn2/NN2CPPNGenome.hpp"
#include "simulatedER/nn2/sq_cppn_genome.hpp"
#include "simulatedER/sq_genome.hpp"

enum morph_genome_type{
    CPPN = 0,
    SQ_CPPN = 1,
    SQ_CG = 2
};

#endif //MORPH_GENOME_HPP