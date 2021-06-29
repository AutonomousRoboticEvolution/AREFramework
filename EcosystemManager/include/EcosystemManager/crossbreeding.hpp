#ifndef CROSSBREEDING_HPP
#define CROSSBREEDING_HPP

#include "EcosystemManager/hybrid_parent_pool.hpp"

namespace are {

template<typename crossover_t>
class Crossbreeding
{
public:
    typedef std::unique_ptr<Crossbreeding> Ptr;
    typedef std::unique_ptr<const Crossbreeding> ConstPtr;

    Crossbreeding(){}
    Crossbreeding(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param):
    parameters(param), randomNum(rn){}

    void init();

    void load_data();

    virtual void generate() = 0;
private:
    HybridParentPool<crossover_t> parent_pool;

    ///set the environment type, evolution type...
    settings::ParametersMapPtr parameters;
    ///random number generator
    misc::RandNum::Ptr randomNum;
};

} //are

#endif //CROSSBREEDING_HPP
