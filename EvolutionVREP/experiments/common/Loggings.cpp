#include "Loggings.h"

using namespace are;

void FitnessLog::saveLog(are::EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

    savePopFile << generation << "," << ea->get_population().size() << ",";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        savePopFile << ea->get_population()[i]->getObjectives()[0] << ",";
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

void NNGenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t i = 0; i < ea->get_population().size(); i++){
        NEAT::Genome genome = std::dynamic_pointer_cast<NNGenome>(ea->getIndividual(i)->get_ctrl_genome())->get_nn_genome();
        std::stringstream filepath;
        filepath << Logging::log_folder << "/genome_" << generation << "_" << i;
        genome.Save(filepath.str().c_str());
    }
}
