#include "HNBOLoggings.h"

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
            std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_final_position())
            logFileStream << pos << ",";
    }
    logFileStream << std::endl;

    logFileStream.close();
}

void BOLog::saveLog(EA::Ptr &ea)
{
    std::string repository = settings::getParameter<settings::String>(ea->get_parameters(),"#repository").value;
    int generation = static_cast<const EA_HyperNEAT*>(ea.get())->get_generation();
    int eval = static_cast<const EA_HyperNEAT*>(ea.get())->getNumberOfEval();
    Eigen::VectorXd obs = static_cast<const EA_HyperNEAT*>(ea.get())->getLastObs();
    Eigen::VectorXd spl = static_cast<const EA_HyperNEAT*>(ea.get())->getLastSpl();

    std::ofstream saveFile;
    saveFile.open(repository + std::string("/") +  Logging::log_folder + std::string("/") + logFile, std::ios::out | std::ios::ate | std::ios::app);
    if(!saveFile)
    {
        std::cerr << "unable to open : " << logFile << std::endl;
	return;
    }

    saveFile << "generation " << generation << ": ,";
    saveFile << "evaluation " << eval << ": ,";
    saveFile << "observation ,";
    for(int i = 0; i < obs.rows(); i++)
	    saveFile << obs(i) << ",";
    saveFile << "sample ,";
    for(int i = 0; i < spl.rows(); i++)
	    saveFile << spl(i) << ",";

    saveFile << std::endl;
    saveFile.close();
}

void LearnerSerialLog::saveLog(EA::Ptr &ea){
	std::string repository = settings::getParameter<settings::String>(ea->get_parameters(),"#repository").value;
	std::stringstream sstream;
    sstream << repository << "/" << Logging::log_folder << "/"
            << logFile << "_" << ea->get_generation()
            << "_" << static_cast<const EA_HyperNEAT*>(ea.get())->get_currentIndIndex();
	limbo::serialize::TextArchive tarch(sstream.str());
	std::dynamic_pointer_cast<BOLearner>(ea->getIndividual(0)->get_learner())->model().save<limbo::serialize::TextArchive>(tarch);
}

