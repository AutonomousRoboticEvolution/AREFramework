#include "simulatedER/mazeEnv.h"
#include "emptyEnv.hpp"
#include "visualization.hpp"
#include "simulatedER/obstacleAvoidance.hpp"
#include "barrelTask.hpp"
#include "simulatedER/exploration.hpp"

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
    else if(env_type == 2)
        env = std::make_shared<are::sim::BarrelTask>(param);
    else if(env_type == 3)
        env = std::make_shared<are::sim::Exploration>(param);
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
