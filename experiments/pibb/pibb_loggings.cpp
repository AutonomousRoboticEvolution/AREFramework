#include "pibb_loggings.hpp"
#include "pibb.hpp"

using namespace are;

void BestIndividualLog::saveLog(EA::Ptr &ea){

    int generation = ea->get_generation();
    const PiBBIndividual::Ptr &current_policy = static_cast<PiBB*>(ea.get())->get_current_policy();

    //log current policy fitness
    std::ofstream fit_ofs;
    std::stringstream fit_filename;
    fit_filename << "policy_fitness.csv";
    if(!openOLogFile(fit_ofs, fit_filename.str()))
        return;
    fit_ofs << generation << "," << current_policy->getObjectives()[0] << std::endl;
    fit_ofs.close();

    //log ctrl genome
    std::ofstream ctrl_ofs;
    std::stringstream filename;
    filename << "policy_params_" << generation;
    if(!openOLogFile(ctrl_ofs, filename.str()))
        return;
    ctrl_ofs << static_cast<PiBB*>(ea.get())->get_pibb()->policy_params();

    ctrl_ofs.close();

    //log trajectory
    int env_type = settings::getParameter<settings::Integer>(ea->get_parameters(),"#envType").value;
    if(env_type == sim::MULTI_TARGETS || env_type == sim::BARREL){
        std::vector<std::vector<are::waypoint>> trajs = current_policy->get_trajectories();
        int i = 0;
        for(auto &traj : trajs){
            std::ofstream traj_ofs;
            std::stringstream filepath;
            filepath << "/traj_" << generation << "_" << i;
            logFile = filepath.str();

            if(!openOLogFile(traj_ofs))
                return;
            for(const are::waypoint& wp: traj)
                traj_ofs << wp.to_string() << std::endl;
            traj_ofs.close();
            i++;
        }
    }
    else if(env_type == sim::GRADUAL){}
    else{
        std::vector<are::waypoint> traj = current_policy->get_trajectory();
        std::ofstream traj_ofs;
        std::stringstream filepath;
        filepath << "/traj_" << generation;
        logFile = filepath.str();

        if(!openOLogFile(traj_ofs))
            return;
        for(const are::waypoint& wp: traj)
            traj_ofs << wp.to_string() << std::endl;
        traj_ofs.close();
    }
    if(env_type == sim::PUSH_OBJECT){
        std::vector<are::waypoint> traj = current_policy->get_object_trajectory();
        std::ofstream traj_ofs;
        std::stringstream filepath;
        filepath << "/obj_traj_" << generation;
        logFile = filepath.str();

        if(!openOLogFile(traj_ofs))
            return;
        for(const are::waypoint& wp: traj)
            traj_ofs << wp.to_string() << std::endl;
        traj_ofs.close();
    }

    //log rollout
    std::stringstream roll_filepath;
    roll_filepath << Logging::log_folder << "/rollout_" << generation;
    std::ofstream roll_filestream(roll_filepath.str());
    rollout_t rollout = current_policy->get_rollout();
    for(const act_obs_sample& aos: rollout)
        roll_filestream << aos.to_string() << std::endl;
    roll_filestream.close();

}

void TensorGenomeLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();


    for(int i = 0; i < ea->get_pop_size() - 1; i++){
        std::ofstream genome_ofs;
        std::stringstream filename;
        filename << "policy_params_" << generation << "_" << i;
        if(!openOLogFile(genome_ofs, filename.str()))
            return;

        TensorGenome::Ptr tg = std::dynamic_pointer_cast<TensorGenome>(ea->get_population()[i]->get_ctrl_genome());
        genome_ofs << tg->tensor();
        genome_ofs.close();
    }
}
