#include "testEnv.h"
#include "EA_HyperNEAT.h"

extern "C" are::Environment::Ptr environmentFactory(const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::TestEnv);
    env->set_parameters(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::EA_HyperNEAT(st));

    ea->set_randomNum(rn);
    return ea;
}
