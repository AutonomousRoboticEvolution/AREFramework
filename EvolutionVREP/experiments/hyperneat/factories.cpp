#include "mazeEnv.h"
#include "EA_HyperNEAT.h"
#include "Loggings.h"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::MazeEnv);
    env->set_parameters(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::EA_HyperNEAT(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr log(new are::FitnessLog(fit_log_file));
    logs.push_back(log);
}
