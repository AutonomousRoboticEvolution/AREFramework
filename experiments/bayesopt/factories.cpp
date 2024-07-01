#include "simulatedER/mazeEnv.h"
#include "BO.hpp"
#include "BOLoggings.hpp"
#include "BOIndividual.hpp"
#include "barrelTask.hpp"
#include "obstacleAvoidance.hpp"
#include "locomotion.hpp"
#include "barrelTask.hpp"
#include "multiTargetMaze.hpp"
#include "env_settings.hpp"

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
    else if(env_type == are::sim::MULTI_TARGETS)
        env = std::make_shared<are::sim::MultiTargetMaze>(param);
    else if(env_type == are::sim::LOCOMOTION)
        env = std::make_shared<are::sim::Locomotion>(param);
    else if(env_type == are::sim::BARREL)
        env = std::make_shared<are::sim::BarrelTask>(param);
    else
        std::cerr << "factory env: unknown environement" << std::endl;

    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::BO>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string bo_log_file = are::settings::getParameter<are::settings::String>(param,"#BOSamplesFile").value;
    are::BOLog::Ptr bolog = std::make_shared<are::BOLog>(bo_log_file);
    logs.push_back(bolog);

    std::string learner_log_file = are::settings::getParameter<are::settings::String>(param,"#LearnerSerialFile").value;
    are::LearnerSerialLog::Ptr lslog = std::make_shared<are::LearnerSerialLog>(learner_log_file);
    logs.push_back(lslog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::BOIndividual>::Ptr bdlog = std::make_shared<are::BehavDescLog<are::BOIndividual>>(behav_desc_log_file);
    logs.push_back(bdlog);

//    std::string eval_time_log_file = are::settings::getParameter<are::settings::String>(param,"#evalTimeFile").value;
//    are::EvalTimeLog::Ptr etlog(new are::EvalTimeLog(eval_time_log_file));
//    std::dynamic_pointer_cast<are::EvalTimeLog>(etlog)
//            ->set_end_of_gen(are::settings::getParameter<are::settings::Integer>(param,"#instanceType").value == 1);
//    logs.push_back(etlog);
}

