#include "simulatedER/mazeEnv.h"
#include "NSGC.hpp"
#include "ARE/Logging.h"
#include "simulatedER/Logging.hpp"

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

    ea.reset(new are::NSGC(st));

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
    are::BehavDescLog<are::NSGCIndividual>::Ptr bdlog(new are::BehavDescLog<are::NSGCIndividual>(behav_desc_log_file));
    logs.push_back(bdlog);

    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
    are::ArchiveLog<are::NSGC>::Ptr arclog(new are::ArchiveLog<are::NSGC>(archive_log_file));
    logs.push_back(arclog);

    std::string ec_log_file = are::settings::getParameter<are::settings::String>(param,"#energyCostFile").value;
    are::EnergyCostLog<are::NSGCIndividual>::Ptr eclog(new are::EnergyCostLog<are::NSGCIndividual>(ec_log_file));
    logs.push_back(eclog);

    std::string st_log_file = are::settings::getParameter<are::settings::String>(param,"#simTimeFile").value;
    are::SimTimeLog<are::NSGCIndividual>::Ptr stlog(new are::SimTimeLog<are::NSGCIndividual>(st_log_file));
    logs.push_back(stlog);

    are::TrajectoryLog<are::NSGCIndividual>::Ptr trajlog(new are::TrajectoryLog<are::NSGCIndividual>);
    logs.push_back(trajlog);

    are::NNParamGenomeLog::Ptr nnglog(new are::NNParamGenomeLog);
    logs.push_back(nnglog);
}

