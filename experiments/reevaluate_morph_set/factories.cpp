#include "ARE/mazeEnv.h"
#include "reevaluate_morph_set.hpp"
#include "ARE/Logging.h"

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

    ea.reset(new are::ReevaluateMorphSet(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
    logs.push_back(fitlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::NN2Individual>::Ptr bdlog(new are::BehavDescLog<are::NN2Individual>(behav_desc_log_file));
    logs.push_back(bdlog);

    are::TrajectoryLog<are::NN2Individual>::Ptr trajlog(new are::TrajectoryLog<are::NN2Individual>);
    logs.push_back(trajlog);

//    std::string energy_cost_log_file = are::settings::getParameter<are::settings::String>(param,"#energyCostFile").value;
//    are::EnergyCostLog<are::NN2Individual>::Ptr eclog(new are::EnergyCostLog<are::NN2Individual>(energy_cost_log_file));
//    logs.push_back(eclog);

    std::string sim_time_log_file = are::settings::getParameter<are::settings::String>(param,"#simTimeFile").value;
    are::SimTimeLog<are::NN2Individual>::Ptr stlog(new are::SimTimeLog<are::NN2Individual>(sim_time_log_file));
    logs.push_back(stlog);

}

