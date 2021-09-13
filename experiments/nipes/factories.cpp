#include "simulatedER/mazeEnv.h"
#include "NIPES.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "NIPESLoggings.hpp"
#include "simulatedER/Logging.hpp"
#include "obstacleAvoidance.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
    are::Environment::Ptr env;
    if(env_type == 0){
        env.reset(new are::sim::MazeEnv);
        env->set_parameters(param);
    }
    else if(env_type == 1)
        env.reset(new are::sim::ObstacleAvoidance(param));
    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::NIPES(rn,st));
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
    etlog->set_end_of_gen(are::settings::getParameter<are::settings::Integer>(param,"#instanceType").value == 1);
    logs.push_back(etlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::sim::NN2Individual>::Ptr bdlog(new are::BehavDescLog<are::sim::NN2Individual>(behav_desc_log_file));
    logs.push_back(bdlog);

    are::NNParamGenomeLog::Ptr nnpglog(new are::NNParamGenomeLog);
    logs.push_back(nnpglog);

    are::TrajectoryLog<are::sim::NN2Individual>::Ptr trajlog(new are::TrajectoryLog<are::sim::NN2Individual>);
    logs.push_back(trajlog);

    std::string stop_crit_log_file = are::settings::getParameter<are::settings::String>(param,"#stopCritFile").value;
    are::StopCritLog::Ptr sclog(new are::StopCritLog(stop_crit_log_file));
    logs.push_back(sclog);

    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
    are::ArchiveLog<are::NIPES>::Ptr arclog(new are::ArchiveLog<are::NIPES>(archive_log_file));
    logs.push_back(arclog);

    std::string ec_log_file = are::settings::getParameter<are::settings::String>(param,"#energyCostFile").value;
    are::EnergyCostLog<are::sim::NN2Individual>::Ptr eclog(new are::EnergyCostLog<are::sim::NN2Individual>(ec_log_file));
    logs.push_back(eclog);

    std::string st_log_file = are::settings::getParameter<are::settings::String>(param,"#simTimeFile").value;
    are::SimTimeLog<are::sim::NN2Individual>::Ptr stlog(new are::SimTimeLog<are::sim::NN2Individual>(st_log_file));
    logs.push_back(stlog);

}

