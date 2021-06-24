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
    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
    logs.push_back(fitlog);

    are::MorphGenomeLog::Ptr mglog(new are::MorphGenomeLog());
    logs.push_back(mglog);

    std::string md_log_file = are::settings::getParameter<are::settings::String>(param,"#morphDescFile").value;
    are::MorphDescCartWHDLog::Ptr mdlog(new are::MorphDescCartWHDLog(md_log_file));
    logs.push_back(mdlog);

    are::ControllersLog::Ptr ctrllog(new are::ControllersLog());
    logs.push_back(ctrllog);

    are::NNParamGenomeLog::Ptr ctrlGenLog(new are::NNParamGenomeLog);
    logs.push_back(ctrlGenLog);

    bool use_ctrl_arch = are::settings::getParameter<are::settings::Boolean>(param,"#useControllerArchive").value;
    if(use_ctrl_arch){
    are::ControllerArchiveLog::Ptr calog(new are::ControllerArchiveLog());
    logs.push_back(calog);
    }

    are::TrajectoryLog<are::M_NIPESIndividual>::Ptr trajLog(new are::TrajectoryLog<are::M_NIPESIndividual>);
    logs.push_back(trajLog);

}


