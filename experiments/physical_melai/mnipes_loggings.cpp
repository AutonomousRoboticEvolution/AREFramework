#include "mnipes_loggings.hpp"

using namespace are;


void RobotInfoLog::saveLog(EA::Ptr &ea)
{
    int id = static_cast<MNIPES*>(ea.get())->get_currentID();
    int eval = static_cast<MNIPES*>(ea.get())->get_numberEvaluation();
    std::stringstream sstr;
    sstr << "robot_infos_" << id << "_" << eval;
    std::ofstream ifs;
    if(!openOLogFile(ifs,sstr.str())){
        std::cerr << "Unable to open file : " << sstr.str() << std::endl;
        return;
    }

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

