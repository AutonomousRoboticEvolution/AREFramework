#include "simulatedER/mazeEnv.h"
#include "NSLC.hpp"
#include "ARE/Logging.h"

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

    ea.reset(new are::NSLC(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
    logs.push_back(fitlog);

    std::string eval_time_log_file = are::settings::getParameter<are::settings::String>(param,"#evalTimeFile").value;
    are::EvalTimeLog::Ptr etlog(new are::EvalTimeLog(eval_time_log_file));
    logs.push_back(etlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::NSLCIndividual>::Ptr bdlog(new are::BehavDescLog<are::NSLCIndividual>(behav_desc_log_file));
    logs.push_back(bdlog);


    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
    are::ArchiveLog<are::NSLC>::Ptr arclog(new are::ArchiveLog<are::NSLC>(archive_log_file));
    logs.push_back(arclog);


    are::NNParamGenomeLog::Ptr nnglog(new are::NNParamGenomeLog);
    logs.push_back(nnglog);
}

