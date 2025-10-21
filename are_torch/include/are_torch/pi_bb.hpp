#pragma once

#include "ARE/Learner.h"
#include "ARE/Individual.h"
#include "ARE/misc/RandNum.h"
#include <torch/torch.h>

namespace are{
namespace learning{
/**
 * @brief PiBB from the paper F. Stulp and O. Sigaud, “Policy improvement methods: Between blackbox optimization and episodic reinforcement learning,” Arch. ouverte HAL, vol. 1, p. 34, Oct. 2012.
 * and following the implementation of "Generic Neural Locomotion Control  Framework for Legged Robots", M. Thor et al., 2021
 */
class PiBB: public Learner{
public:
    typedef std::unique_ptr<PiBB> Ptr;
    typedef std::unique_ptr<const PiBB> ConstPtr;

    PiBB() : Learner(){}

    void init(int sample_size, int nbr_params, double sigma, bool maximisation = false);
    void update(Control::Ptr& ctrl) override{};
    void generate_samples(torch::Tensor &params);
    void step(const rollout_t&, std::vector<double> returns, double reward);
    bool iterate();

    torch::Tensor& policy_params(){return _policy_params;}

    int get_sample_size(){return _sample_size;}
    void set_rand_num(const misc::RandNum::Ptr rand_num){_rand_num = rand_num;}
    void set_inverse_lambda(double il){_inverse_lambda=il;}

private:
    misc::RandNum::Ptr _rand_num;
    std::vector<rollout_t> _rollouts;
    torch::Tensor _rewards;
    double _inverse_lambda;
    double _sigma;
    int _sample_size;
    torch::Tensor _policy_params;
    torch::Tensor _exp_noise;
    torch::Tensor _trajectory_costs;
    torch::Tensor _probabilities;
    int _current_sample=0;
    bool _maximisation = false;
};
}//learning
}//are
