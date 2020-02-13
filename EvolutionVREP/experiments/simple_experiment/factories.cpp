#include "misc/RandNum.h"
#include "ARE/Settings.h"
#include "DummyEnv.h"
#include "DummyControl.h"
#include "ARE/Genome.h"



extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::DummyEnv::Ptr env(new are::DummyEnv);
    env->set_parameters(param);
    return env;
}




extern "C" are::EA::Ptr EAFactory
    (const misc::RandNum::Ptr &rn,
     const are::settings::ParametersMapPtr &st)
{

    return ea;
}
