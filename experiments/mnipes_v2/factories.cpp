#include "simulatedER/mazeEnv.h"
#include "M_NIPES.hpp"
#include "MNIPESLoggings.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::sim::MazeEnv);
    env->set_parameters(param);
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


