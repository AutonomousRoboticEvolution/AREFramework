#include "frictionSetup.hpp"


extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::FrictionSetUp);
    env->set_parameters(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::FricEA(rn,st));
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fric_log_file = are::settings::getParameter<are::settings::String>(param,"#fricLogFile").value;
    are::FricCoeffLog::Ptr friclog(new are::FricCoeffLog(fric_log_file));
    logs.push_back(friclog);
}

