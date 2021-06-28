#include "MNIPESLoggings.hpp"

using namespace are;




void MorphGenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        std::stringstream filepath;
        filepath << Logging::log_folder << "/morphGenome_" << generation << "_" <<  ind;
        const Genome::Ptr morphGenome = std::dynamic_pointer_cast<M_NIPESIndividual>(ea->getIndividual(ind))->get_morph_genome();
        NEAT::Genome genome = std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
        genome.Save(filepath.str().c_str());
    }
}

void MorphDescCartWHDLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        logFileStream << generation << "," << ind << ",";
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<M_NIPESIndividual>(ea->getIndividual(ind))->getMorphDesc();
        for(int j = 0; j < morphDesc.size(); j++){
            logFileStream << morphDesc(j) << ",";
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}

void ControllersLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();

    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "controllers_" << generation << "_" << i;
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<CMAESLearner>(
                             ea->get_population()[i]->get_learner()
                             )->archive_to_string();
        logFileStream.close();
    }
}

void ControllerArchiveLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();
    std::ofstream logFileStream;
    std::stringstream filename;
    filename << "controller_archive" << generation;
    if(!openOLogFile(logFileStream, filename.str()))
        return;
    for(size_t i = 0; i < static_cast<M_NIPES*>(ea.get())->get_controller_archive().size(); i++)
    {
        for(size_t j = 0; j < static_cast<M_NIPES*>(ea.get())->get_controller_archive()[i].size(); j++)
        {
            for(size_t k = 0; k < static_cast<M_NIPES*>(ea.get())->get_controller_archive()[i][j].size(); k++)
            {
                logFileStream << i << "," << j << "," << k << std::endl;
                logFileStream << static_cast<M_NIPES*>(ea.get())->get_controller_archive()[i][j][k].first->to_string();
                logFileStream << static_cast<M_NIPES*>(ea.get())->get_controller_archive()[i][j][k].second << std::endl;
            }
        }
    }
    logFileStream.close();
}
