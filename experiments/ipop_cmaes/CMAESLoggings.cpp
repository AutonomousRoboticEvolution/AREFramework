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

void NoveltyLog::saveLog(are::EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

    savePopFile << generation << "," << ea->get_population().size() << ",";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        savePopFile << ea->get_population()[i]->getObjectives()[1] << ",";
    }

    savePopFile << std::endl;
    savePopFile.close();
}

void ArchiveLog::saveLog(are::EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

    Eigen::VectorXd desc;

    savePopFile << generation << ";" << static_cast<CMAES*>(ea.get())->get_archive().size() << ";";
    for (size_t i = 0; i < static_cast<CMAES*>(ea.get())->get_archive().size(); i++) {
        desc = static_cast<CMAES*>(ea.get())->get_archive()[i];
        for(size_t j = 0; j < desc.rows() - 1 ; j++)
            savePopFile << desc(j) << ",";
        savePopFile << desc(desc.rows()-1) << ";";
    }

    savePopFile << std::endl;
    savePopFile.close();
}
