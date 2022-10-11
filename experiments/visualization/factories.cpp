#include "simulatedER/mazeEnv.h"
#include "emptyEnv.hpp"
#include "visualization.hpp"
#include "obstacleAvoidance.hpp"
#include "barrelTask.hpp"
#include "exploration.hpp"

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
    else if(env_type == 2)
        env.reset(new are::sim::BarrelTask(param));
    else if(env_type == 3)
        env.reset(new are::sim::Exploration(param));
    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::Visu(rn,st));
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{

}
