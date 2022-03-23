#include "simulatedER/mazeEnv.h"
#include "obstacleAvoidance.hpp"
#include "M_NIPES.hpp"
#include "MNIPESLoggings.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
    are::Environment::Ptr env;
    if(env_type == 0){
        env.reset(new are::sim::MazeEnv);
        env->set_parameters(param);
    }
    else if(env_type == 1)
        env.reset(new are::sim::ObstacleAvoidance(param));
    else if(env_type == 2)
        env.reset(new are::sim::MultiTargetMaze(param));

    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;
    ea.reset(new are::M_NIPES(rn,st));
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::GenomeInfoLog::Ptr gilog(new are::GenomeInfoLog(fit_log_file));
    logs.push_back(gilog);


    bool use_ctrl_arch = are::settings::getParameter<are::settings::Boolean>(param,"#useControllerArchive").value;
    if(use_ctrl_arch){
        are::ControllerArchiveLog::Ptr calog(new are::ControllerArchiveLog());
        logs.push_back(calog);
    }

}


