#ifndef LOCOMOTION_PPO_HPP
#define LOCOMOTION_PPO_HPP

#include "ARE/EA.h"
#include "ARE/Individual.h"
#include "simulatedER/FixedMorphology.hpp"

#include "torchrl/algorithms/ppo/PPO.hpp"
#include "torchrl/algorithms/ppo/PPOArgs.hpp"
#include "torchrl/envs/VectorizedEnv.hpp"
#include "torchrl/rl/RolloutBuffer.hpp"
#include "torchrl/utils/Logger.hpp"

#include "locomotion.hpp"

namespace are{

class PPOInd : public Individual
{
public:

    typedef std::shared_ptr<PPOInd> Ptr;
    typedef std::shared_ptr<const PPOInd> ConstPtr;

    PPOInd() : Individual(), rollout_buffer(1){}
    PPOInd(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen, const PPOArgs& a)
        : Individual(morph_gen,ctrl_gen), args(a), rollout_buffer(1)
    {
    }
    PPOInd(const PPOInd& ind) :
        Individual(ind),
        rollout_buffer(ind.rollout_buffer),
        args(ind.args),
        trajectory(ind.trajectory),
        nb_sensors(ind.nb_sensors),
        nb_wheels(ind.nb_wheels),
        nb_joints(ind.nb_joints)
    {}
    Individual::Ptr clone() override{
        return std::make_shared<PPOInd>(*this);
    };
    void update(double delta_time) override;


    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & morphGenome;
        arch & individual_id;
    }

    std::string to_string() override;
    void from_string(const std::string &str) override;

    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}

    RolloutBuffer &get_rollouts(){return rollout_buffer;}
    Policy& get_policy(){return policy;}
    std::unique_ptr<torch::optim::Adam> &get_optimizer(){return optimizer;}
private:
    void createMorphology() override;
    void createController() override;
    std::vector<waypoint> trajectory;
    int nb_sensors = 0;
    int nb_wheels = 0;
    int nb_joints = 0;

    PPOArgs args;

    Policy policy{nullptr};
    RolloutBuffer rollout_buffer;
    std::unique_ptr<torch::optim::Adam> optimizer;


};

/**
 * @brief implementation of PPO RL algorithm. Copied from https://github.com/adepierre/torchRL.
 */
class RL_PPO : public EA
{
public:
    RL_PPO() : EA(){}
    RL_PPO(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}

    void init() override;
    bool is_finish() override;
    bool update(const Environment::Ptr&) override;

private:

    //std::tuple<float, uint64_t, uint64_t> _collect_rollouts(RolloutBuffer& buffer);

    VectorizedEnv env;
    PPOArgs args;


    uint64_t timestep = 0;
    uint64_t iteration = 0;
    bool has_average = false;

    std::unique_ptr<Logger> logger;
    std::chrono::steady_clock::time_point start_time;
};

}//are

#endif //LOCOMOTION_PPO_HPP
