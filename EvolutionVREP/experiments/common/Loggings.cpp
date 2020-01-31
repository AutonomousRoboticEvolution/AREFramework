#include "Loggings.h"

using namespace are;

void FitnessLog::saveLog(are::EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

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
    float bestFitness = ea->get_population()[0]->getFitness();
    for (size_t i = 1; i < ea->get_population().size(); i++) {
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

void EvalTimeLog::saveLog(EA::Ptr &ea){

    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int nbEval = ea->get_numberEvaluation();
    std::chrono::nanoseconds eval_time = ea->getEvalCompTime();

    logFileStream << nbEval <<  ", " << eval_time.count() << std::endl;

    logFileStream.close();
}

