#include "ARE/Environment.h"
#include "dummyEA.hpp"
#include "ARE/Logging.h"


extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::DummyEnv);
    env->set_parameters(param);
    return env;
}

extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea(new are::DummyEA(rn,st));
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
//    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
//    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
//    logs.push_back(fitlog);

//    std::string eval_time_log_file = are::settings::getParameter<are::settings::String>(param,"#evalTimeFile").value;
//    are::EvalTimeLog::Ptr etlog(new are::EvalTimeLog(eval_time_log_file));
//    logs.push_back(etlog);
}

