#include "MNIPESLoggings.hpp"

using namespace are;




void GenomeInfoLog::saveLog(EA::Ptr &ea)
{
    const genome_t& genome = static_cast<M_NIPES*>(ea.get())->get_gene_pool().back();


    //Log the morph genome
    std::stringstream morph_filepath;
    morph_filepath << Logging::log_folder << "/morph_genome_" << genome.morph_genome.id();
    if(boost::filesystem::exists(morph_filepath.str()))
        return;

    std::ofstream mofstr(morph_filepath.str());
    boost::archive::text_oarchive oarch(mofstr);
    oarch << genome.morph_genome.get_cppn();
    mofstr.close();
    //-

    //Log the ctrl genome
    if(!genome.ctrl_genome.get_weights().empty()){
        std::stringstream ctrl_filepath;
        ctrl_filepath << Logging::log_folder << "/ctrl_genome_" << genome.morph_genome.id();
        std::ofstream cofstr(ctrl_filepath.str());
        cofstr << genome.ctrl_genome.to_string();
        cofstr.close();
    }
    //-

    //Log the fitness and parents ids
    std::ofstream fitness_file_stream;
    if(!openOLogFile(fitness_file_stream))
        return;
    fitness_file_stream << genome.morph_genome.id() << ",";
    for(const int& id: genome.morph_genome.get_parents_ids())
        fitness_file_stream << id << ",";
    for(const double& obj: genome.objectives)
        fitness_file_stream << obj << ",";
    fitness_file_stream << "\n";
    //-
}

void MorphDescCartWHDLog::saveLog(EA::Ptr &ea)
{

    const genome_t& genome = static_cast<M_NIPES*>(ea.get())->get_gene_pool().back();
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    logFileStream << genome.morph_genome.id() << ",";
    Eigen::VectorXd morphDesc = genome.morph_genome.get_morph_desc().getCartDesc();
    for(int j = 0; j < morphDesc.size(); j++){
        logFileStream << morphDesc(j) << ",";
    }
    logFileStream << std::endl;
    logFileStream.close();
}

void ControllersLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    const std::vector<learner_t>& learning_pool = static_cast<M_NIPES*>(ea.get())->get_learning_pool();
    for(const learner_t &learner : learning_pool){
        if(!learner.ctrl_learner.is_learning_finish())
            continue;

        std::stringstream filename;
        filename << "controllers_" << learner.morph_genome.id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << learner.ctrl_learner.archive_to_string();
        logFileStream.close();
    }

}

void ControllerArchiveLog::saveLog(EA::Ptr &ea){

    std::ofstream logFileStream;
    std::stringstream filename;
    filename << "controller_archive";
    logFileStream.open(Logging::log_folder + std::string("/")  + filename.str());
    if(!logFileStream)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/")  + filename.str() << std::endl;
        return;
    }
    logFileStream << static_cast<M_NIPES*>(ea.get())->get_controller_archive_obj().to_string();
    logFileStream.close();

}
