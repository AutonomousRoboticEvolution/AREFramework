#pragma once

#include "simulatedER/nn2/NN2CPPNGenome.hpp"
#include "simulatedER/nn2/sq_cppn_genome.hpp"
#include "simulatedER/sq_genome.hpp"
#include "simulatedER/nn2/dual_cppn_genome.hpp"

enum morph_genome_type{
    CPPN = 0,
    SQ_CPPN = 1,
    SQ_CG = 2,
    DUAL_CPPN = 3
};
