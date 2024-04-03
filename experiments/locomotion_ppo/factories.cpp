#include "locomotion.hpp"
#include "locomotion_ppo.hpp"
#include "ARE/Logging.h"


extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    return std::make_unique<are::sim::Locomotion>(param);
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::RL_PPO>(rn,st);;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{

}
