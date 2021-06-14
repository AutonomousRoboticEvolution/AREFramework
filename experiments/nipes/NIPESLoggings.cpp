#include "NIPESLoggings.hpp"

using namespace are;


void StopCritLog::saveLog(EA::Ptr &ea)
{
    if(!static_cast<NIPES*>(ea.get())->restarted())
        return;

    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int generation = ea->get_generation();

    logFileStream << generation << "," << static_cast<NIPES*>(ea.get())->pop_stopping_criterias() << std::endl;

    logFileStream.close();
}




