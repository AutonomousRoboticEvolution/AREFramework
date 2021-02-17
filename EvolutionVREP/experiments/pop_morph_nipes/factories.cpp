#include "ARE/mazeEnv.h"
<<<<<<< HEAD
#include "straightLineEnv.hpp"
=======
>>>>>>> cfad5f6ea4243a58ac61ad027f95ad1a08cb8dc1
#include "PMNIPES.hpp"
#include "NIPESLoggings.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int envType = are::settings::getParameter<are::settings::Integer>(param,"#environment").value;
    are::Environment::Ptr env;
    if(envType == 0){
        env.reset(new are::MazeEnv);
        env->set_parameters(param);
    }else if(envType == 1){
        env.reset(new are::StraightLine);
        env->set_parameters(param);
    }else{
        std::cerr << "unknown environment : 0 for Maze or 1 for Straight Line" << std::endl;
    }
    return env;
}



extern "C" are::EA::Ptr EAFactory(const misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::PMNIPES(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::morph_log::FitnessLog::Ptr fitlog(new are::morph_log::FitnessLog(fit_log_file));
    logs.push_back(fitlog);

    std::string eval_time_log_file = are::settings::getParameter<are::settings::String>(param,"#evalTimeFile").value;
    are::morph_log::EvalTimeLog::Ptr etlog(new are::morph_log::EvalTimeLog(eval_time_log_file));
    etlog->set_end_of_gen(are::settings::getParameter<are::settings::Integer>(param,"#instanceType").value == 1);
    logs.push_back(etlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::morph_log::BehavDescLog<are::PMNIPESIndividual>::Ptr bdlog(new are::morph_log::BehavDescLog<are::PMNIPESIndividual>(behav_desc_log_file));
    logs.push_back(bdlog);

    are::morph_log::NNParamGenomeLog::Ptr nnpglog(new are::morph_log::NNParamGenomeLog);
    logs.push_back(nnpglog);

    are::morph_log::TrajectoryLog<are::PMNIPESIndividual>::Ptr trajlog(new are::morph_log::TrajectoryLog<are::PMNIPESIndividual>);
    logs.push_back(trajlog);

    std::string stop_crit_log_file = are::settings::getParameter<are::settings::String>(param,"#stopCritFile").value;
    are::morph_log::StopCritLog::Ptr sclog(new are::morph_log::StopCritLog(stop_crit_log_file));
    logs.push_back(sclog);

    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
    are::morph_log::ArchiveLog<are::PMNIPES>::Ptr arclog(new are::morph_log::ArchiveLog<are::PMNIPES>(archive_log_file));
    logs.push_back(arclog);

    std::string ec_log_file = are::settings::getParameter<are::settings::String>(param,"#energyCostFile").value;
    are::morph_log::EnergyCostLog<are::PMNIPESIndividual>::Ptr eclog(new are::morph_log::EnergyCostLog<are::PMNIPESIndividual>(ec_log_file));
    logs.push_back(eclog);

    std::string st_log_file = are::settings::getParameter<are::settings::String>(param,"#simTimeFile").value;
    are::morph_log::SimTimeLog<are::PMNIPESIndividual>::Ptr stlog(new are::morph_log::SimTimeLog<are::PMNIPESIndividual>(st_log_file));
    logs.push_back(stlog);

    are::morph_log::ControllerArchiveLog::Ptr calog(new are::morph_log::ControllerArchiveLog());
    logs.push_back(calog);
}

