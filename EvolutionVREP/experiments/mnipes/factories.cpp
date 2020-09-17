#include "ARE/mazeEnv.h"
#include "M_NIPES.hpp"

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

    ea.reset(new are::M_NIPES(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
//    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
//    are::morph_log::FitnessLog::Ptr fitlog(new are::morph_log::FitnessLog(fit_log_file));
//    logs.push_back(fitlog);

//    std::string eval_time_log_file = are::settings::getParameter<are::settings::String>(param,"#evalTimeFile").value;
//    are::morph_log::EvalTimeLog::Ptr etlog(new are::morph_log::EvalTimeLog(eval_time_log_file));
//    etlog->set_end_of_gen(are::settings::getParameter<are::settings::Integer>(param,"#instanceType").value == 1);
//    logs.push_back(etlog);

//    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
//    are::morph_log::BehavDescLog<are::M_NIPESIndividual>::Ptr bdlog(new are::morph_log::BehavDescLog<are::M_NIPESIndividual>(behav_desc_log_file));
//    logs.push_back(bdlog);

//    are::morph_log::NNParamGenomeLog::Ptr nnpglog(new are::morph_log::NNParamGenomeLog);
//    logs.push_back(nnpglog);

//    are::morph_log::TrajectoryLog<are::M_NIPESIndividual>::Ptr trajlog(new are::morph_log::TrajectoryLog<are::M_NIPESIndividual>);
//    logs.push_back(trajlog);

//    std::string stop_crit_log_file = are::settings::getParameter<are::settings::String>(param,"#stopCritFile").value;
//    are::morph_log::StopCritLog::Ptr sclog(new are::morph_log::StopCritLog(stop_crit_log_file));
//    logs.push_back(sclog);

//    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
//    are::morph_log::ArchiveLog<are::M_NIPES>::Ptr arclog(new are::morph_log::ArchiveLog<are::M_NIPES>(archive_log_file));
//    logs.push_back(arclog);

//    std::string ec_log_file = are::settings::getParameter<are::settings::String>(param,"#energyCostFile").value;
//    are::morph_log::EnergyCostLog<are::M_NIPESIndividual>::Ptr eclog(new are::morph_log::EnergyCostLog<are::M_NIPESIndividual>(ec_log_file));
//    logs.push_back(eclog);

//    std::string st_log_file = are::settings::getParameter<are::settings::String>(param,"#simTimeFile").value;
//    are::morph_log::SimTimeLog<are::M_NIPESIndividual>::Ptr stlog(new are::morph_log::SimTimeLog<are::M_NIPESIndividual>(st_log_file));
//    logs.push_back(stlog);

}

