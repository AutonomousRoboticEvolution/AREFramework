#include "are_torch/pi_bb.hpp"

using namespace are::learning;


void PiBB::init(int sample_size, int nbr_params, double sigma,  bool maximisation){
    _trajectory_costs = torch::zeros({sample_size},torch::TensorOptions().dtype(torch::kDouble));
    _probabilities = torch::zeros({sample_size},torch::TensorOptions().dtype(torch::kDouble));
    _exp_noise = torch::zeros({sample_size,nbr_params},torch::TensorOptions().dtype(torch::kDouble));
    _sigma = sigma;
    _sample_size = sample_size;
    _maximisation = maximisation;
}

void PiBB::step(const rollout_t& rollout, std::vector<double> returns, double reward){
    _rollouts.push_back(rollout);

    torch::Tensor final_return = torch::scalar_tensor(reward,
                                                      torch::TensorOptions().dtype(torch::kDouble));

    torch::Tensor immediate_returns =
        torch::from_blob(returns.data(),
                        {static_cast<int64_t>(returns.size())},
                        torch::TensorOptions().dtype(torch::kDouble)).clone();
    // auto traj_costs = _trajectory_costs.accessor<double,1>();
    if(_maximisation){
        immediate_returns = -immediate_returns;
        final_return = -final_return;
    }
    _trajectory_costs[_current_sample] = final_return + torch::sum(immediate_returns)/immediate_returns.size(0);
    _current_sample++;
}

bool PiBB::iterate(){
    // std::cout << "trajectory costs: " << _trajectory_costs << std::endl;

    torch::Tensor exp_tc = torch::exp(-_inverse_lambda*_trajectory_costs);
    if(torch::isnan(exp_tc).any().item<bool>()){
        std::cerr << "Error PiBB::iterate: NaN in exp_tc" << std::endl;
        return false;
    }
    if(torch::isinf(exp_tc).any().item<bool>()){
        std::cout << "trajectory costs: " << _trajectory_costs << std::endl;
        std::cerr << "Error PiBB::iterate: Inf in exp_tc" << std::endl;
        return false;
    }
    _probabilities = exp_tc/torch::sum(exp_tc);
    if(torch::isnan(_probabilities).any().item<bool>()){
        std::cout << "trajectory costs: " << _trajectory_costs << std::endl;
        std::cerr << "Error PiBB::iterate: NaN in probabilities" << std::endl;
        return false;
    }
    // std::cout << "proba: " << _probabilities << std::endl;
    torch::Tensor delta = torch::sum(torch::mul(torch::transpose(_exp_noise,0,1),_probabilities),1);
    if(torch::isnan(_probabilities).any().item<bool>()){
        std::cerr << "Error PiBB::iterate: NaN in probabilities" << std::endl;
        return false;
    }
    // std::cout << "delta: " << delta << std::endl;
    _policy_params += delta;
    _policy_params = torch::clamp(_policy_params,-1,1);

    return true;
}

void PiBB::generate_samples(torch::Tensor &samples){
    _current_sample = 0;
    samples = torch::zeros({_sample_size,_exp_noise.size(1)},torch::TensorOptions().dtype(torch::kDouble));
    for(int i = 0; i < _sample_size; i++){
        for(int j = 0; j < _exp_noise.size(1); j++){
            _exp_noise[i][j] = _rand_num->normalDist(0,_sigma);
        }
    }
    samples = torch::clamp(_policy_params + _exp_noise,-1,1);

}
