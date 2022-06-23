#include "image_processing/real_environment.hpp"
#include "exploration.hpp"
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
        env = std::make_shared<are::sim::Exploration>(param);
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
}

