#include "image_processing/real_environment.hpp"
#include "exploration.hpp"
#include "barrelTask.hpp"
#include "simulatedER/mazeEnv.h"
#include "multiTargetMaze.hpp"
#include "gradual_env.hpp"
#include "obstacleAvoidance.hpp"
#include "mnipes.hpp"
#include "ARE/Logging.h"
#include "mnipes_loggings.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    bool sim_mode = are::settings::getParameter<are::settings::Boolean>(param,"#simMode").value;
    are::Environment::Ptr env;
    if(!sim_mode){
        env = std::make_shared<are::RealEnvironment>();
        env->set_parameters(param);
    }
    else{
        int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
        if(env_type == are::MAZE){
            env = std::make_shared<are::sim::MazeEnv>();
            env->set_parameters(param);
        }
        else if(env_type == are::OBSTACLES)
            env = std::make_shared<are::sim::ObstacleAvoidance>(param);
        else if(env_type == are::MULTI_TARGETS)
            env = std::make_shared<are::sim::MultiTargetMaze>(param);
        else if(env_type == are::EXPLORATION)
            env = std::make_shared<are::sim::Exploration>(param);
        else if(env_type == are::BARREL)
            env = std::make_shared<are::sim::BarrelTask>(param);
        else if(env_type == are::GRADUAL)
            env = std::make_shared<are::sim::GradualEnvironment>(param);
    }
   return env;
}


extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::MNIPES(rn,st));
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    are::RobotInfoLog::Ptr robInfoLog(new are::RobotInfoLog);
    logs.push_back(robInfoLog);

    are::LearnerStateLog::Ptr learnerStateLog(new are::LearnerStateLog);
    logs.push_back(learnerStateLog);

    are::BestControllerLog::Ptr bestControllerLog(new are::BestControllerLog);
    logs.push_back(bestControllerLog);

    if(are::settings::getParameter<are::settings::Boolean>(param,"#useControllerArchive").value){
        are::ControllerArchiveLog::Ptr ctrlArchLog(new are::ControllerArchiveLog);
        logs.push_back(ctrlArchLog);
    }


}

