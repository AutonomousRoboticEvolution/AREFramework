#include "BOLoggings.h"

using namespace are;

void BehavDescLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int numberEval = ea->get_numberEvaluation();
    std::vector<double> final_position = static_cast<const noEA*>(ea.get())->get_final_position();

    logFileStream << numberEval << ",";
    for(const double &pos : final_position)
            logFileStream << pos << ",";

    logFileStream << std::endl;

    logFileStream.close();
}


void BOLog::saveLog(EA::Ptr &ea)
{
    std::string repository = settings::getParameter<settings::String>(ea->get_parameters(),"#repository").value;
    int eval = static_cast<const noEA*>(ea.get())->getNumberOfEval();
    Eigen::VectorXd obs = static_cast<const noEA*>(ea.get())->getLastObs();
    Eigen::VectorXd spl = static_cast<const noEA*>(ea.get())->getLastSpl();

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
    int dataset_size = std::dynamic_pointer_cast<BOLearner>(ea->getIndividual(0)->get_learner())->dataset_size();
    std::stringstream sstream;
    sstream << Logging::log_folder << "/" << logFile << "_" << ea->get_generation() << "_" << dataset_size;
    limbo::serialize::TextArchive tarch(sstream.str());
    std::dynamic_pointer_cast<BOLearner>(ea->getIndividual(0)->get_learner())->model().save<limbo::serialize::TextArchive>(tarch);
}
