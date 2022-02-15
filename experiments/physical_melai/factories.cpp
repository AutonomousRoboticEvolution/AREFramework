#include "image_processing/real_environment.hpp"
#include "mnipes.hpp"
#include "ARE/Logging.h"
#include "mnipes_loggings.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::RealEnvironment());
    env->set_parameters(param);
    return env;
}


extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::MNIPES(rn,st));
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    are::RobotInfoLog::Ptr robInfoLog(new are::RobotInfoLog);
    logs.push_back(robInfoLog);
}

