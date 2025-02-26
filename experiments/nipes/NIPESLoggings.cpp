#include "NIPESLoggings.hpp"

using namespace are;


void StopCritLog::saveLog(EA::Ptr &ea)
{
    if(!static_cast<NIPES*>(ea.get())->restarted())
        return;

    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int generation = ea->get_generation();

    logFileStream << generation << "," << static_cast<NIPES*>(ea.get())->pop_stopping_criterias() << std::endl;

    logFileStream.close();
}

void BestIndividualLog::saveLog(EA::Ptr &ea){

    if(!static_cast<NIPES*>(ea.get())->is_healthy_generation())
        return;

    int generation = ea->get_generation();
    auto &best_ind = static_cast<NIPES*>(ea.get())->get_best_individual();

    //log ctrl genome
    std::ofstream ctrl_ofs;

    std::stringstream filename;
    filename << "ctrlGenome_" << generation << "_" << best_ind.first;
    if(!openOLogFile(ctrl_ofs, filename.str()))
        return;
    ctrl_ofs << std::dynamic_pointer_cast<NNParamGenome>(
                             best_ind.second->get_ctrl_genome())->to_string();

    ctrl_ofs.close();

    //log trajectory
    int env_type = settings::getParameter<settings::Integer>(ea->get_parameters(),"#envType").value;
    if(env_type == sim::MULTI_TARGETS || env_type == sim::BARREL){
        std::vector<std::vector<are::waypoint>> trajs = std::dynamic_pointer_cast<NIPESIndividual>(best_ind.second)->get_trajectories();
        int i = 0;
        for(auto &traj : trajs){
            std::ofstream traj_ofs;
            std::stringstream filepath;
            filepath << "/traj_" << generation << "_" << best_ind.first << "_" << i;
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
        std::vector<are::waypoint> traj = std::dynamic_pointer_cast<NIPESIndividual>(best_ind.second)->get_trajectory();
        std::ofstream traj_ofs;
        std::stringstream filepath;
        filepath << "/traj_" << generation << "_" << best_ind.first;
        logFile = filepath.str();

        if(!openOLogFile(traj_ofs))
            return;
        for(const are::waypoint& wp: traj)
            traj_ofs << wp.to_string() << std::endl;
        traj_ofs.close();
    }
    if(env_type == sim::PUSH_OBJECT){
        std::vector<are::waypoint> traj = std::dynamic_pointer_cast<NIPESIndividual>(best_ind.second)->get_object_trajectory();
        std::ofstream traj_ofs;
        std::stringstream filepath;
        filepath << "/obj_traj_" << generation << "_" << best_ind.first;
        logFile = filepath.str();

        if(!openOLogFile(traj_ofs))
            return;
        for(const are::waypoint& wp: traj)
            traj_ofs << wp.to_string() << std::endl;
        traj_ofs.close();
    }

}
