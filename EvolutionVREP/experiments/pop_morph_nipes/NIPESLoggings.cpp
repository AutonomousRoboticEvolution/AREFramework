#include "NIPESLoggings.hpp"

using namespace are;


void morph_log::StopCritLog::saveLog(EA::Ptr &ea)
{
    if(!static_cast<PMNIPES*>(ea.get())->restarted())
        return;

    logFile = static_cast<PMNIPES*>(ea.get())->subFolder() + std::string("/") + log_file;

    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int generation = ea->get_generation();

    logFileStream << generation << "," << static_cast<PMNIPES*>(ea.get())->pop_stopping_criterias() << std::endl;

    logFileStream.close();
}




void morph_log::FitnessLog::saveLog(are::EA::Ptr &ea)
{
    int generation = ea->get_generation();
    logFile = static_cast<PMNIPES*>(ea.get())->subFolder() + std::string("/") + log_file;

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

    savePopFile << generation << "," << ea->get_population().size() << "," << ea->get_population()[0]->getObjectives().size() << ",";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        for (const double& obj : ea->get_population()[i]->getObjectives()) {
            savePopFile << obj << ",";
        }
    }
    savePopFile << std::endl;
    savePopFile.close();
}

void morph_log::EvalTimeLog::saveLog(EA::Ptr &ea){

    logFile = static_cast<PMNIPES*>(ea.get())->subFolder() + std::string("/") + log_file;


    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int nbEval = ea->get_numberEvaluation();
    std::chrono::nanoseconds eval_time = ea->getEvalCompTime();

    logFileStream << nbEval <<  ", " << eval_time.count() << std::endl;

    logFileStream.close();
}

void morph_log::NNParamGenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << static_cast<PMNIPES*>(ea.get())->subFolder() << "/genome_" << generation << "_" << i;
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<NNParamGenome>(
                             ea->get_population()[i]->get_ctrl_genome()
                             )->to_string();
        logFileStream.close();
    }
}

void morph_log::ControllerArchiveLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();
    std::ofstream logFileStream;
    std::stringstream filename;
    filename << "controller_archive" << generation;
    if(!openOLogFile(logFileStream, filename.str()))
        return;
    for(size_t i = 0; i < static_cast<PMNIPES*>(ea.get())->get_controller_archive().size(); i++)
    {
        for(size_t j = 0; j < static_cast<PMNIPES*>(ea.get())->get_controller_archive()[i].size(); j++)
        {
            for(size_t k = 0; k < static_cast<PMNIPES*>(ea.get())->get_controller_archive()[i][j].size(); k++)
            {
                logFileStream << i << "," << j << "," << k << std::endl;
                logFileStream << static_cast<PMNIPES*>(ea.get())->get_controller_archive()[i][j][k].first->to_string();
                logFileStream << static_cast<PMNIPES*>(ea.get())->get_controller_archive()[i][j][k].second << std::endl;
            }
        }
    }
    logFileStream.close();
}
