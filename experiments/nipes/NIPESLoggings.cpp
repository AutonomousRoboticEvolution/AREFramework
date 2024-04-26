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
