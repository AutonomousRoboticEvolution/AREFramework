#include "meim_logging.hpp"

using namespace are;

void GenomeInfoLog::saveLog(EA::Ptr &ea)
{
    int genome_type = settings::getParameter<settings::Boolean>(ea->get_parameters(),"#morphGenomeType").value;
    for(const auto &genome:  static_cast<MEIM*>(ea.get())->get_new_genes()){

        //- Log the cppn
        if(genome_type == morph_genome_type::CPPN || genome_type == morph_genome_type::SQ_CPPN){
            std::stringstream morph_filepath;
            morph_filepath << Logging::log_folder << "/cppn_" << genome.morph_genome->id();
            if(boost::filesystem::exists(morph_filepath.str()))
                continue;
            std::ofstream mofstr(morph_filepath.str());
            boost::archive::text_oarchive oarch(mofstr);
            if(genome_type == morph_genome_type::CPPN)
                oarch << std::dynamic_pointer_cast<NN2CPPNGenome>(genome.morph_genome)->get_cppn();
            else if(genome_type == morph_genome_type::SQ_CPPN)
                oarch << std::dynamic_pointer_cast<SQCPPNGenome>(genome.morph_genome)->get_cppn();
            mofstr.close();
        }
        //-

        //- Log the superquadric
        if(genome_type == morph_genome_type::SQ_CPPN || genome_type == morph_genome_type::SQ_CG){
            std::string quadric_file = settings::getParameter<settings::String>(ea->get_parameters(),"#quadricFile").value;
            std::ofstream qofstr;
            if(!openOLogFile(qofstr,quadric_file))
                return;
            qofstr << genome.morph_genome->id() << ";";
            if(genome_type == morph_genome_type::SQ_CPPN)
                qofstr << std::dynamic_pointer_cast<SQCPPNGenome>(genome.morph_genome)->get_quadric().to_string();
            else if(genome_type == morph_genome_type::SQ_CG)
                qofstr << std::dynamic_pointer_cast<SQGenome>(genome.morph_genome)->get_quadric().to_string();
            qofstr << std::endl;
            qofstr.close();
        }
        if(genome_type == morph_genome_type::SQ_CG){
            std::string cg_file = settings::getParameter<settings::String>(ea->get_parameters(),"#compGenFile").value;
            std::ofstream cgofstr;
            if(!openOLogFile(cgofstr,cg_file))
                return;
            cgofstr << genome.morph_genome->id() << "-"
                    << std::dynamic_pointer_cast<SQGenome>(
                           genome.morph_genome
                           )->get_components_genome().to_string() << std::endl;
            cgofstr.close();
        }


        //- Log morph features <width,depth,height,voxels,wheels,sensor,joint,caster>
        std::ofstream mfofs;
        if(!openOLogFile(mfofs,"/morph_features.csv"))
            return;
        mfofs << genome.morph_genome->id() << ",";
        Eigen::VectorXd morph_feat;
        if(genome_type == morph_genome_type::CPPN)
            morph_feat = std::dynamic_pointer_cast<NN2CPPNGenome>(genome.morph_genome)->get_feat_desc().to_eigen_vector();
        else if(genome_type == morph_genome_type::SQ_CPPN)
            morph_feat = std::dynamic_pointer_cast<SQCPPNGenome>(genome.morph_genome)->get_feat_desc().to_eigen_vector();
        else if(genome_type == morph_genome_type::SQ_CG)
            morph_feat = std::dynamic_pointer_cast<SQGenome>(genome.morph_genome)->get_feat_desc().to_eigen_vector();

        mfofs << morph_feat(0);
        for(size_t j = 1; j < morph_feat.size(); j++){
            mfofs << "," << morph_feat(j);
        }
        mfofs << std::endl;
        mfofs.close();
        //-

        //- Log morph organ position descriptor: matrix of 11x11x11
        std::ofstream mdofs;
        if(!openOLogFile(mdofs,"/morph_descriptor.csv"))
            return;
        mdofs << genome.morph_genome->id() << "," << morphology_constants::real_matrix_size << ",";
        Eigen::VectorXd morph_desc;
        if(genome_type == morph_genome_type::CPPN)
            morph_desc = std::dynamic_pointer_cast<NN2CPPNGenome>(genome.morph_genome)->get_matrix_desc().to_eigen_vector();
        else if(genome_type == morph_genome_type::SQ_CPPN)
            morph_desc = std::dynamic_pointer_cast<SQCPPNGenome>(genome.morph_genome)->get_matrix_desc().to_eigen_vector();
        else if(genome_type == morph_genome_type::SQ_CG)
            morph_desc = std::dynamic_pointer_cast<SQGenome>(genome.morph_genome)->get_matrix_desc().to_eigen_vector();

        mdofs << morph_desc(0);
        for(size_t j = 1; j < morph_desc.size(); j++){
            mdofs << "," <<  morph_desc(j);
        }
        mdofs << std::endl;
        mdofs.close();
        //-

        //plot the trajectory
        std::stringstream traj_filepath;
        traj_filepath << Logging::log_folder << "/traj_" << genome.morph_genome->id();
        std::ofstream tofs(traj_filepath.str());
        for(const auto &wp: genome.trajectory)
            tofs << wp.to_string() << std::endl;
        tofs.close();

        //- Log the fitness values and parents ids
        std::ofstream fitness_file_stream;
        if(!openOLogFile(fitness_file_stream))
            return;
        fitness_file_stream << genome.morph_genome->id() << ",";
        for(const int& id: genome.morph_genome->get_parents_ids())
            fitness_file_stream << id << ",";
        for(const double& obj: genome.objectives)
            fitness_file_stream << obj << ",";
        fitness_file_stream << std::endl;
        fitness_file_stream.close();
        //-

        //- Rollout logging
        std::stringstream roll_filepath;
        roll_filepath << Logging::log_folder << "/rollout_" << genome.morph_genome->id();
        std::ofstream roll_filestream(roll_filepath.str());
        for(const act_obs_sample& aos: genome.rollout)
            roll_filestream << aos.to_string() << std::endl;
        roll_filestream.close();
        //-
    }
    static_cast<MEIM*>(ea.get())->clear_new_genes();

}

void ParentsPoolLog::saveLog(EA::Ptr &ea){
    int pop_size = settings::getParameter<settings::Integer>(ea->get_parameters(),"#populationSize").value;
    const std::vector<genome_t>& genomes = static_cast<MEIM*>(ea.get())->get_parent_pool();
    if(genomes.size() != pop_size)
        return;
    std::ifstream ifs(Logging::log_folder + std::string("/")  + logFile);
    std::string l,line;
    if(ifs)
        while(std::getline(ifs,l))
        line = l;


    std::stringstream sstr;
    sstr << genomes.front().morph_genome->id();
    for(size_t i = 1; i < genomes.size(); i++)
        sstr << "," << genomes.at(i).morph_genome->id();
    if(!line.empty() && sstr.str() == line) //genome pool didn't change
        return;
    std::ofstream ofs;
    if(!openOLogFile(ofs))
        return;
    ofs << sstr.str() << std::endl;
    ofs.close();
}


