#include "ARE/Genome.h"

using namespace are;

Genome::Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param, int id)
{
    _id = id;
    randomNum = rn;
    parameters = param;
    parents_ids= std::vector<int>(2,-1);

}


Genome::~Genome() {
    parameters.reset();
    randomNum.reset();
}

void Genome::from_file(const std::string &filename){
    std::ifstream logFileStream;
    logFileStream.open(filename);
    if(!logFileStream){
        std::cerr << "Genome::from_file - unable to open " << filename << std::endl;
        return;
    }
    std::string gen_str = "",line;
    while(std::getline(logFileStream,line))
        gen_str = gen_str + line + "\n";
    logFileStream.close();
    from_string(gen_str);
}

