#include "misc/RandNum.h"
#include "ARE/Settings.h"
#include "DummyEnv.h"
#include "ARE/CAT.h"
#include "DummyControl.h"
#include "ARE/Genome.h"
#include "ARE/FixedBaseMorphology.h"
#include "ARE/EA_SteadyState.h"
#include "ARE/EA_MultiNEAT.h"



extern "C" std::shared_ptr<Environment> environmentFactory(const std::shared_ptr<Settings>& st)
{
   std::shared_ptr<Environment> env(new DummyEnv);
    env->settings = st;
    return env;
}


extern "C" std::shared_ptr<Morphology> morphologyFactory(int type,std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
    std::shared_ptr<Morphology> fixedBaseMorph(new FixedBaseMorphology);
    return fixedBaseMorph;
}

extern "C" std::shared_ptr<Control> controlFactory(int type,std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
    std::shared_ptr<Control> ctrl(new DummyControl);
    ctrl->randomNum = rn;
    ctrl->settings = st;
    return ctrl;
}

extern "C" std::shared_ptr<Genome> genomeFactory(int type,std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
    std::shared_ptr<Genome> dfgenome(new DefaultGenome(rn,st));
    return dfgenome;
}

extern "C" std::shared_ptr<EA> EAFactory(std::shared_ptr<RandNum> rn, std::shared_ptr<Settings> st)
{
    std::shared_ptr<EA> ea;
    if(st->evolutionType == Settings::STEADY_STATE)
        ea.reset(new EA_SteadyState(*st));
    else if (st->evolutionType == Settings::EA_MULTINEAT)
        ea.reset(new EA_MultiNEAT(*st));

    ea->randomNum = rn;
    return ea;
}
