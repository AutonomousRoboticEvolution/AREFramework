#include "simulatedER/mazeEnv.h"
#include "pibb.hpp"
#include "pibb_loggings.hpp"
#include "simulatedER/Logging.hpp"
#include "env_settings.hpp"
#include "obstacleAvoidance.hpp"
#include "barrelTask.hpp"
#include "exploration.hpp"
#include "locomotion.hpp"
#include "hill_climbing.hpp"
#include "push_object.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
    are::Environment::Ptr env;
    if(env_type == are::sim::MAZE){
        env = std::make_shared<are::sim::MazeEnv>();
        env->set_parameters(param);
    }
    else if(env_type == are::sim::OBSTACLES)
        env = std::make_shared<are::sim::ObstacleAvoidance>(param);
    // else if(env_type == are::sim::MULTI_TARGETS)
    //     env = std::make_shared<are::sim::MultiTargetMaze>(param);
    else if(env_type == are::sim::LOCOMOTION)
        env = std::make_shared<are::sim::Locomotion>(param);
    else if(env_type == are::sim::BARREL)
        env = std::make_shared<are::sim::BarrelTask>(param);
    else if(env_type == are::sim::HILL_CLIMBING)
        env = std::make_shared<are::sim::HillClimbing>(param);
    else if(env_type == are::sim::PUSH_OBJECT)
        env = std::make_shared<are::sim::PushObject>(param);
    else
        std::cerr << "factory env: unknown environement" << std::endl;

    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::PiBB>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog = std::make_shared<are::FitnessLog>(fit_log_file);
    logs.push_back(fitlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::PiBBIndividual>::Ptr bdlog = std::make_shared<are::BehavDescLog<are::PiBBIndividual>>(behav_desc_log_file);
    logs.push_back(bdlog);

    are::BestIndividualLog::Ptr bilog = std::make_shared<are::BestIndividualLog>();
    logs.push_back(bilog);
}

