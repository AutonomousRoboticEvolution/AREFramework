#include "ARE/Genome.h"

using namespace are;

Genome::Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param, int id)
{
    _id = id;
    randomNum = rn;
    parameters = param;
}


Genome::~Genome() {
    parameters.reset();
    randomNum.reset();
}

void Genome::from_file(const std::string &filename){
    std::ifstream logFileStream;
    logFileStream.open(filename);

    std::string gen_str = "",line;
    while(std::getline(logFileStream,line))
        gen_str = gen_str + line + "\n";

    from_string(gen_str);
}

