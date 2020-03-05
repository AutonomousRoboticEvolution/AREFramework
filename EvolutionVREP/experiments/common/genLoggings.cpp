#include "genLoggings.hpp"

using namespace are;

void NNParamGenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "genome_" << generation << "_" << i;
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<NNParamGenome>(
                             ea->get_population()[i]->get_ctrl_genome()
                             )->to_string();
        logFileStream.close();
    }
}

void NNParamGenomeLog::loadLog(const std::string &log_file, EA::Ptr &ea){
    std::ifstream logFileStream;

    if(!openILogFile(logFileStream, log_file))
        return;

//    logFileStream.
    std::string gen_str;
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
