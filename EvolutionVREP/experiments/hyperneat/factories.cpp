#include "testEnv.h"
#include "EA_HyperNEAT.h"

extern "C" are::Environment::Ptr environmentFactory(const are::Settings::Ptr& st)
{
    are::Environment::Ptr env(new are::TestEnv);
    env->settings = st;
    return env;
}

extern "C" are::EA::Ptr EAFactory(misc::RandNum::Ptr rn, are::Settings::Ptr st)
{
    are::EA::Ptr ea;

    ea.reset(new are::EA_HyperNEAT(*st));

    ea->set_randomNum(rn);
    return ea;
}
