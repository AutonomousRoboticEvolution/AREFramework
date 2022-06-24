#include "mnipes_loggings.hpp"

using namespace are;


void RobotInfoLog::saveLog(EA::Ptr &ea)
{
    int id = static_cast<MNIPES*>(ea.get())->get_currentID();
    int eval = static_cast<MNIPES*>(ea.get())->get_learners().at(id).nbr_eval();
    std::stringstream sstr;
    sstr << "robot_infos_" << id;
    std::ofstream ifs;
    if(!openOLogFile(ifs,sstr.str())){
        std::cerr << "Unable to open file : " << sstr.str() << std::endl;
        return;
    }
    ifs << "evaluation_" << eval << std::endl;
    ifs << "fitnesses;";
    for(double& obj: static_cast<MNIPES*>(ea.get())->get_objectives())
        ifs << obj << ";";
    ifs << std::endl;

    ifs << "trajectory;" << std::endl;
    for(auto& wp: static_cast<MNIPES*>(ea.get())->get_trajectory()){
        ifs << wp.to_string() << std::endl;
    }
    ifs.close();
}


void LearnerStateLog::saveLog(EA::Ptr &ea)
{
    int id = static_cast<MNIPES*>(ea.get())->get_currentID();
    int eval = static_cast<MNIPES*>(ea.get())->get_learners().at(id).nbr_eval();
    int iter = static_cast<MNIPES*>(ea.get())->get_learners().at(id).nbr_iter();
    std::cout << static_cast<MNIPES*>(ea.get())->get_learners().at(id).print_info() << std::endl;
    std::stringstream sstr;
    sstr << "learner_" << id << "_" << iter << "_" << eval;
    std::ofstream ifs;
    if(!openOLogFile(ifs,sstr.str())){
        std::cerr << "Unable to open file : " << sstr.str() << std::endl;
        return;
    }
    ifs << static_cast<MNIPES*>(ea.get())->get_learners().at(id).to_string();
    ifs.close();
}

void BestControllerLog::saveLog(EA::Ptr &ea)
{
    int id = static_cast<MNIPES*>(ea.get())->get_currentID();
    if(!static_cast<MNIPES*>(ea.get())->get_learners().at(id).is_learning_finish())
        return;
    std::stringstream sstr;
    sstr << "/genomes_pool/ctrl_genome_" << id;
    std::ofstream ifs;
    if(!openOLogFile(ifs,sstr.str())){
        std::cerr << "Unable to open file : " << sstr.str() << std::endl;
        return;
    }
    ifs << static_cast<MNIPES*>(ea.get())->get_best_current_ctrl_genome().to_string();
    ifs.close();
}
