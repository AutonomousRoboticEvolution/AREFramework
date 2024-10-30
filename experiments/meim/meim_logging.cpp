#include "meim_logging.hpp"

using namespace are;

void GenomeInfoLog::saveLog(EA::Ptr &ea)
{
    for(const auto &genome:  static_cast<MEIM*>(ea.get())->get_new_genes()){

        //- Log the morph genome
        std::stringstream morph_filepath;
        morph_filepath << Logging::log_folder << "/morph_genome_" << genome.morph_genome->id();
        if(boost::filesystem::exists(morph_filepath.str()))
            continue;
        std::ofstream mofstr(morph_filepath.str());
        boost::archive::text_oarchive oarch(mofstr);
        oarch << genome.morph_genome->get_cppn();
        mofstr.close();
        //-

        //- Log morph features <width,depth,height,voxels,wheels,sensor,joint,caster>
        std::ofstream mfofs;
        if(!openOLogFile(mfofs,"/morph_features.csv"))
            return;
        mfofs << genome.morph_genome->id() << ",";
        Eigen::VectorXd morph_feat = genome.morph_genome->get_cart_desc().getCartDesc();
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
        mdofs << genome.morph_genome->id() << "," << morphology_constants::real_matrix_size << ",";
        Eigen::VectorXd morph_desc = genome.morph_genome->get_organ_position_desc().getCartDesc();
        mdofs << morph_desc(0);
        for(int j = 1; j < morph_desc.size(); j++){
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


