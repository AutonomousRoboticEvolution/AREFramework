#include "mazeEnv.h"
#include "CMAES.hpp"
#include "CMAESIndividual.hpp"
#include "CMAESLoggings.hpp"
#include "genLoggings.hpp"
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

    ea.reset(new are::CMAES(st));

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
    std::dynamic_pointer_cast<are::EvalTimeLog>(etlog)
            ->set_end_of_gen(are::settings::getParameter<are::settings::Integer>(param,"#instanceType").value == 1);
    logs.push_back(etlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog::Ptr bdlog(new are::BehavDescLog(behav_desc_log_file));
    logs.push_back(bdlog);

    are::NNParamGenomeLog::Ptr nnpglog(new are::NNParamGenomeLog);
    logs.push_back(nnpglog);

    std::string stop_crit_log_file = are::settings::getParameter<are::settings::String>(param,"#stopCritFile").value;
    are::StopCritLog::Ptr sclog(new are::StopCritLog(stop_crit_log_file));
    logs.push_back(sclog);

    std::string novelty_log_file = are::settings::getParameter<are::settings::String>(param,"#noveltyFile").value;
    are::NoveltyLog::Ptr nvlog(new are::NoveltyLog(novelty_log_file));
    logs.push_back(nvlog);

    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
    are::ArchiveLog::Ptr arclog(new are::ArchiveLog(archive_log_file));
    logs.push_back(arclog);
}
