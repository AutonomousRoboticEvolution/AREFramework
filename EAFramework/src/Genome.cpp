#include "ARE/Genome.h"

using namespace are;

Genome::Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param)
{
    randomNum = rn;
    parameters = param;
}


Genome::~Genome() {
    parameters.reset();
    randomNum.reset();
}







