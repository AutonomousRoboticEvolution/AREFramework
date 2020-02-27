#include "NELoggings.hpp"

using namespace are;

void BehavDescLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int generation = ea->get_generation();

    logFileStream << "generation," << generation << ",";
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << i << ",";
        for(const double &pos :
            std::dynamic_pointer_cast<NNIndividual>(ea->getIndividual(i))->get_final_position())
            logFileStream << pos << ",";
    }
    logFileStream << std::endl;

    logFileStream.close();
}
