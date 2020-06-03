#include "MB_NIPESLoggings.hpp"

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
            std::dynamic_pointer_cast<NN2Individual>(ea->getIndividual(i))->get_final_position())
            logFileStream << pos << ",";
    }
    logFileStream << std::endl;

    logFileStream.close();
}

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

    savePopFile << generation << ";" << static_cast<NIPES*>(ea.get())->get_archive().size() << ";";
    for (size_t i = 0; i < static_cast<NIPES*>(ea.get())->get_archive().size(); i++) {
        desc = static_cast<NIPES*>(ea.get())->get_archive()[i];
        for(size_t j = 0; j < desc.rows() - 1 ; j++)
            savePopFile << desc(j) << ",";
        savePopFile << desc(desc.rows()-1) << ";";
    }

    savePopFile << std::endl;
    savePopFile.close();
}


void BOLog::saveLog(EA::Ptr &ea)
{
    std::string repository = settings::getParameter<settings::String>(ea->get_parameters(),"#repository").value;
    int eval = ea->get_numberEvaluation();
    Eigen::VectorXd obs = static_cast<const MB_NIPES*>(ea.get())->getLastObs();
    Eigen::VectorXd spl = static_cast<const MB_NIPES*>(ea.get())->getLastSpl();

    std::ofstream saveFile;
    saveFile.open(Logging::log_folder + std::string("/") + logFile, std::ios::out | std::ios::ate | std::ios::app);
    if(!saveFile)
    {
        std::cerr << "unable to open : " << logFile << std::endl;
        return;
    }
    saveFile << "evaluation " << eval << ": ,";
    saveFile << "observation : ,";
    for(int i = 0; i < obs.rows(); i++)
        saveFile << obs(i) << ",";
    saveFile << "sample : ,";
    for(int i = 0; i < spl.rows(); i++)
        saveFile << spl(i) << ",";

    saveFile << std::endl;
    saveFile.close();
}

void LearnerSerialLog::saveLog(EA::Ptr &ea){
    std::string repository = settings::getParameter<settings::String>(ea->get_parameters(),"#repository").value;
    int dataset_size = static_cast<const MB_NIPES*>(ea.get())->get_learner()->dataset_size();
    std::stringstream sstream;
    sstream << Logging::log_folder << "/" << logFile << "_" << ea->get_generation() << "_" << dataset_size;
    limbo::serialize::TextArchive tarch(sstream.str());
    static_cast<const MB_NIPES*>(ea.get())->get_learner()->model().save<limbo::serialize::TextArchive>(tarch);
}
