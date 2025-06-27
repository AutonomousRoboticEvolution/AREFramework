#include "simulatedER/mazeEnv.h"
#include "emptyEnv.hpp"
#include "visualization.hpp"
#include "simulatedER/obstacleAvoidance.hpp"
#include "barrelTask.hpp"
#include "exploration.hpp"
#include "locomotion.hpp"
#include "hill_climbing.hpp"
#include "push_object.hpp"
#include "env_settings.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
    are::Environment::Ptr env;
    if(env_type == are::sim::MAZE){
        env.reset(new are::sim::MazeEnv);
        env->set_parameters(param);
    }
    else if(env_type == are::sim::OBSTACLES)
        env.reset(new are::sim::ObstacleAvoidance(param));
    else if(env_type == are::sim::BARREL)
        env.reset(new are::sim::BarrelTask(param));
    else if(env_type == are::sim::EXPLORATION)
        env.reset(new are::sim::Exploration(param));
    else if(env_type == are::sim::LOCOMOTION)
        env.reset(new are::sim::Locomotion(param));
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
    return std::make_unique<are::Visu>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{

    are::TrajectoryLog<are::VisuInd>::Ptr trajlog = std::make_shared<are::TrajectoryLog<are::VisuInd>>();
    logs.push_back(trajlog);
}
