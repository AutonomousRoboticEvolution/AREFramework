#include "Loggings.h"

using namespace are;

void FitnessLog::saveLog(are::EA::Ptr &ea)
{
    std::string repository = settings::getParameter<settings::String>(ea->get_parameters(),"#repository").value;
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    savePopFile.open(repository + std::string("/") + Logging::log_folder + std::string("/")  + logFile, std::ios::out | std::ios::ate | std::ios::app);
    if(!savePopFile)
    {
        std::cerr << "unable to open : " << logFile << std::endl;
        return;
    }
    savePopFile << "generation " << generation << ": ,";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        savePopFile << " ind " << i << ": " << ea->get_population()[i]->getFitness() << ",";
    }
    float avgFitness = 0;
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        avgFitness += ea->get_population()[i]->getFitness();
    }
    avgFitness = avgFitness / ea->get_population().size();
    savePopFile << "avg: ," << avgFitness << ",";
    int bestInd = 0;
    float bestFitness = 0;
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        if (bestFitness < ea->get_population()[i]->getFitness()) {
            bestFitness = ea->get_population()[i]->getFitness();
            bestInd = i;
        }
    }
    savePopFile << "ind: ," << ea->get_population()[bestInd]->get_individual_id() << ",";
    savePopFile << "fitness: ," << bestFitness << ",";
    savePopFile << "individuals: ,";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        savePopFile << ea->get_population()[i]->get_individual_id() << ",";
    }
    savePopFile << std::endl;
    savePopFile.close();
}
