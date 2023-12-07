#include "simulatedER/mazeEnv.h"
#include "reevaluateFixed.hpp"
#include "ARE/Logging.h"
#include "simulatedER/Logging.hpp"
#include "simulatedER/obstacleAvoidance.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
    are::Environment::Ptr env;
    if(env_type == 0){
        env = std::make_shared<are::sim::MazeEnv>();
        env->set_parameters(param);
    }
    else if(env_type == 1)
        env = std::make_shared<are::sim::ObstacleAvoidance>(param);

    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::ReevaluateFixed>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog = std::make_shared<are::FitnessLog>(fit_log_file);
    logs.push_back(fitlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::sim::NN2Individual>::Ptr bdlog = std::make_shared<are::BehavDescLog<are::sim::NN2Individual>>(behav_desc_log_file);
    logs.push_back(bdlog);

    are::TrajectoryLog<are::sim::NN2Individual>::Ptr trajlog = std::make_shared<are::TrajectoryLog<are::sim::NN2Individual>>();
    logs.push_back(trajlog);

    std::string energy_cost_log_file = are::settings::getParameter<are::settings::String>(param,"#energyCostFile").value;
    are::EnergyCostLog<are::sim::NN2Individual>::Ptr eclog = std::make_shared<are::EnergyCostLog<are::sim::NN2Individual>>(energy_cost_log_file);
    logs.push_back(eclog);

    std::string sim_time_log_file = are::settings::getParameter<are::settings::String>(param,"#simTimeFile").value;
    are::SimTimeLog<are::sim::NN2Individual>::Ptr stlog = std::make_shared<are::SimTimeLog<are::sim::NN2Individual>>(sim_time_log_file);
    logs.push_back(stlog);

}

