#include "MNIPESLoggings.hpp"

using namespace are;

void GenomeInfoLog::saveLog(EA::Ptr &ea)
{
    for(const genome_t& genome : static_cast<M_NIPES*>(ea.get())->get_new_genes()){
        //Log the morph genome
        std::stringstream morph_filepath;
        morph_filepath << Logging::log_folder << "/morph_genome_" << genome.morph_genome.id();
        if(boost::filesystem::exists(morph_filepath.str()))
            continue;

        std::ofstream mofstr(morph_filepath.str());
        boost::archive::text_oarchive oarch(mofstr);
        oarch << genome.morph_genome.get_cppn();
        mofstr.close();
        //-

        //- Log morph features <width,depth,height,voxels,wheels,sensor,joint,caster>
        std::ofstream mfofs;
        if(!openOLogFile(mfofs,"/morph_features.csv"))
            return;
        mfofs << genome.morph_genome.id() << ",";
        Eigen::VectorXd morph_feat = genome.morph_genome.get_cart_desc().getCartDesc();
        mfofs << morph_feat(0);
        for(int j = 1; j < morph_feat.size(); j++){
            mfofs << "," << morph_feat(j);
        }
        mfofs << std::endl;
        mfofs.close();
        //-

        //- Log morph organ position descriptor: matrix of 11x11x11
        std::ofstream mdofs;
        if(!openOLogFile(mdofs,"/morph_descriptor.csv"))
            return;
        mdofs << genome.morph_genome.id() << "," << morphology_constants::real_matrix_size << ",";
        Eigen::VectorXd morph_desc = genome.morph_genome.get_organ_position_desc().getCartDesc();
        mdofs << morph_desc(0);
        for(int j = 1; j < morph_desc.size(); j++){
            mdofs << "," <<  morph_desc(j);
        }
        mdofs << std::endl;
        mdofs.close();
        //-

        //Log the ctrl genome
        if(!genome.ctrl_genome.get_weights().empty()){
            std::stringstream ctrl_filepath;
            ctrl_filepath << Logging::log_folder << "/ctrl_genome_" << genome.morph_genome.id();
            std::ofstream cofstr(ctrl_filepath.str());
            cofstr << genome.ctrl_genome.to_string();
            cofstr.close();

            //If there is a ctrl genome there is a trajectory and a behavioural descriptor
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
        fitness_file_stream << genome.nbr_eval << "," << genome.learning_progress;

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
    static_cast<M_NIPES*>(ea.get())->clear_new_genes();
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

        if(learner.ctrl_learner.get_archive().empty())
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
    int pop_size = settings::getParameter<settings::Integer>(ea->get_parameters(),"#populationSize").value;

    const std::vector<genome_t>& genomes = static_cast<M_NIPES*>(ea.get())->get_gene_pool();
    if(genomes.size() < pop_size)
        return;
    std::ifstream ifs(Logging::log_folder + std::string("/")  + logFile);
    std::string l,line;
    if(ifs)
        while(std::getline(ifs,l))
		line = l;
	

    std::stringstream sstr;
    sstr << genomes.front().morph_genome.id();
    for(size_t i = 1; i < genomes.size(); i++)
        sstr << "," << genomes.at(i).morph_genome.id();
    if(!line.empty() && sstr.str() == line) //genome pool didn't change
        return;
    std::ofstream ofs;
    if(!openOLogFile(ofs))
        return;
    ofs << sstr.str() << std::endl;
    ofs.close();
}

void BestGenomesArchiveLog::saveLog(EA::Ptr &ea){
    if(!boost::filesystem::exists(Logging::log_folder + "/best_genomes_archive"))
        boost::filesystem::create_directories(Logging::log_folder + "/best_genomes_archive");
    if(static_cast<M_NIPES*>(ea.get())->get_best_gene_archive().empty())
        return;
    const genome_t& genome = static_cast<M_NIPES*>(ea.get())->get_best_gene_archive().back();
    int id = genome.morph_genome.id();

    std::stringstream morph_folderpath;
    morph_folderpath << Logging::log_folder << "/best_genomes_archive/morph_" << id;
    if(boost::filesystem::exists(morph_folderpath.str()))
        return;
    boost::filesystem::create_directories(morph_folderpath.str());

    //Log the morph genome
    std::stringstream morph_filepath;
    morph_filepath << morph_folderpath.str() << "/morph_genome_" << id;
    if(boost::filesystem::exists(morph_filepath.str()))
        return;

    std::ofstream mofstr(morph_filepath.str());
    if(!mofstr){
        std::cerr << "Unable to open file " << morph_filepath.str() << std::endl;
        return;
    }
    boost::archive::text_oarchive oarch(mofstr);
    oarch << genome.morph_genome.get_cppn();
    mofstr.close();
    //-

    //Log descriptor
    std::stringstream md_filepath;
    md_filepath << morph_folderpath.str() << "/morph_descriptors_" << id <<  ".csv";
    std::ofstream mdofs(md_filepath.str());
    if(!mdofs){
        std::cerr << "Unable to open file " << md_filepath.str() << std::endl;
        return;
    }
    Eigen::VectorXd morphDesc = genome.morph_genome.get_cart_desc().getCartDesc();
    for(int j = 0; j < morphDesc.size(); j++){
        mdofs << morphDesc(j) << ",";
    }
    mdofs << std::endl;
    mdofs.close();

    //Log the ctrl genome
    std::stringstream ctrl_filepath;
    ctrl_filepath << morph_folderpath.str() << "/ctrl_genome_" << id;
    std::ofstream cofstr(ctrl_filepath.str());
    if(!cofstr){
        std::cerr << "Unable to open file " << ctrl_filepath.str() << std::endl;
        return;
    }
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
        traj_filepath << morph_folderpath.str() << "/traj_" << id << "_" << i;
        std::ofstream tofs(traj_filepath.str());
        if(!tofs){
            std::cerr << "Unable to open file " << traj_filepath.str() << std::endl;
            return;
        }
        for(const auto &wp: traj)
            tofs << wp.to_string() << std::endl;
        tofs.close();
        i++;
    }

    //Log descriptor
    std::stringstream bd_filepath;
    bd_filepath << morph_folderpath.str() << "/behavioral_descriptors_" << id;
    std::ofstream dofs(bd_filepath.str());
    if(!dofs)
        return;
    dofs << genome.behavioral_descriptor << std::endl;
    dofs.close();

    //-

    //Log the fitness and parents ids
    std::stringstream fit_filepath;
    fit_filepath << morph_folderpath.str() << "/fitnesses_" << id;
    std::ofstream fofs(fit_filepath.str());
    if(!fofs){
        std::cerr << "Unable to open file " << fit_filepath.str() << std::endl;
        return;
    }

    fofs << genome.morph_genome.id() << ",";
    for(const int& id: genome.morph_genome.get_parents_ids())
        fofs << id << ",";
    for(const double& obj: genome.objectives)
        fofs << obj << ",";
    fofs << genome.nbr_eval << "," << genome.learning_progress;
    fofs << "\n";
    fofs.close();
    //-

    //Update genomes info file
    std::ofstream giofs(Logging::log_folder + "/best_genomes_archive/" + logFile,std::ios::out | std::ios::ate | std::ios::app);
    if(!giofs){
        std::cerr << "Unable to open file " << Logging::log_folder <<  "/best_genomes_archive/" << logFile << std::endl;
        return;
    }

    giofs << "morph_genome_" << id << std::endl;
    giofs << "fitness,float," << genome.objectives[0] <<std::endl;
    giofs << "real,bool,0" << std::endl;
    giofs << "task,string," << genome.task << std::endl;
    giofs << "environment,string," << genome.environment << std::endl;
    giofs.close();


}
