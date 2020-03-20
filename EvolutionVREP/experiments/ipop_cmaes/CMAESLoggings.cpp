#include "CMAESLoggings.hpp"

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
            std::dynamic_pointer_cast<CMAESIndividual>(ea->getIndividual(i))->get_final_position())
            logFileStream << pos << ",";
    }
    logFileStream << std::endl;

    logFileStream.close();
}

void StopCritLog::saveLog(EA::Ptr &ea)
{
    if(!static_cast<CMAES*>(ea.get())->restarted())
        return;

    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int generation = ea->get_generation();

    logFileStream << generation << "," << static_cast<CMAES*>(ea.get())->pop_stopping_criterias() << std::endl;

    logFileStream.close();
}
