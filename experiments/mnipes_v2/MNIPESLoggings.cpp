#include "MNIPESLoggings.hpp"

using namespace are;




void GenomeInfoLog::saveLog(EA::Ptr &ea)
{
    if(static_cast<M_NIPES*>(ea.get())->get_gene_pool().empty())
        return;
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

    //Log descriptor
    std::ofstream mdofs;
    if(!openOLogFile(mdofs,"/morph_descriptors.csv"))
        return;
    mdofs << genome.morph_genome.id() << ",";
    Eigen::VectorXd morphDesc = genome.morph_genome.get_cart_desc().getCartDesc();
    for(int j = 0; j < morphDesc.size(); j++){
        mdofs << morphDesc(j) << ",";
    }
    mdofs << std::endl;
    mdofs.close();

    //Log the ctrl genome
    if(!genome.ctrl_genome.get_weights().empty()){
        std::stringstream ctrl_filepath;
        ctrl_filepath << Logging::log_folder << "/ctrl_genome_" << genome.morph_genome.id();
        std::ofstream cofstr(ctrl_filepath.str());
        cofstr << genome.ctrl_genome.to_string();
        cofstr.close();

        //If there is a ctrl genome there are a trajectory and a behavioural descriptor
        //Log Trajectory
        int i = 0;
        for(const auto &traj: genome.trajectories)
        {
            if(traj.empty()){
                i++;
                continue;
            }
            std::stringstream traj_filepath;
            traj_filepath << Logging::log_folder << "/traj_" << genome.morph_genome.id() << "_" << i;
            std::ofstream tofs(traj_filepath.str());
            for(const auto &wp: traj)
                tofs << wp.to_string() << std::endl;
            tofs.close();
            i++;
        }

        //Log descriptor
        std::ofstream dofs;
        if(!openOLogFile(dofs,"/behavioral_descriptors"))
            return;
        dofs << genome.morph_genome.id() << std::endl;
        dofs << genome.behavioral_descriptor << std::endl;
        dofs.close();
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
    fitness_file_stream << genome.nbr_eval;
    if(!genome.rewards.empty()){
        fitness_file_stream << "," << genome.rewards.front();
        for(size_t i = 1; i < genome.rewards.size(); i++)
            fitness_file_stream << "," << genome.rewards[i];
    }
    fitness_file_stream << "\n";
    fitness_file_stream.close();
    //-

    //Log the id and the total number of evaluations in the whole evolution used before finishing the training of this robot
    std::ofstream neofs;
    if(!openOLogFile(neofs,"/number_of_evaluations.csv"))
        return;
    neofs << genome.morph_genome.id() << "," << ea->get_numberEvaluation() << "\n";
    neofs.close();
}

void MorphDescCartWHDLog::saveLog(EA::Ptr &ea)
{
    const genome_t& genome = static_cast<M_NIPES*>(ea.get())->get_gene_pool().back();
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    logFileStream << genome.morph_genome.id() << ",";
    Eigen::VectorXd morphDesc = genome.morph_genome.get_cart_desc().getCartDesc();
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

void GenomesPoolLog::saveLog(EA::Ptr &ea){
    const std::vector<genome_t>& genomes = static_cast<M_NIPES*>(ea.get())->get_gene_pool();
    if(genomes.empty())
        return;
    std::ofstream ofs;
    if(!openOLogFile(ofs))
        return;
    ofs << genomes.front().morph_genome.id();
    for(size_t i = 1; i < genomes.size(); i++)
        ofs << "," << genomes.at(i).morph_genome.id();
    ofs << std::endl;
    ofs.close();
}
