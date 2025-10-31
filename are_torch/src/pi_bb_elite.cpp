#include "are_torch/pi_bb_elite.hpp"

using namespace are::learning;


void PiBBElite::init(int sample_size, int nbr_params, double sigma, double inverse_lambda, double sigma_decay, double lambda_decay, double elite_ratio,  bool maximisation){
    _trajectory_costs = torch::zeros({sample_size},torch::TensorOptions().dtype(torch::kDouble));
    _probabilities = torch::zeros({static_cast<int64_t>(sample_size*elite_ratio)},torch::TensorOptions().dtype(torch::kDouble));
    _exp_noise = torch::zeros({sample_size,nbr_params},torch::TensorOptions().dtype(torch::kDouble));
    _elite_costs = torch::ones({static_cast<int64_t>(sample_size*elite_ratio)},torch::TensorOptions().dtype(torch::kDouble))*100;
    _samples = torch::zeros({sample_size,nbr_params},torch::TensorOptions().dtype(torch::kDouble));
    _elite_samples =  torch::zeros({static_cast<int64_t>(sample_size*elite_ratio),nbr_params},torch::TensorOptions().dtype(torch::kDouble));

    _sigma = sigma;
    _inverse_lambda = inverse_lambda;
    _lambda_decay = lambda_decay;
    _sigma_decay = sigma_decay;
    _sample_size = sample_size;
    _maximisation = maximisation;
    _elite_ratio = elite_ratio;
}

void PiBBElite::step(const rollout_t& rollout, std::vector<double> returns, double reward){
    _rollouts.push_back(rollout);

    torch::Tensor final_return = torch::scalar_tensor(reward,
                                                      torch::TensorOptions().dtype(torch::kDouble));

    torch::Tensor immediate_returns =
        torch::from_blob(returns.data(),
                        {static_cast<int64_t>(returns.size())},
                        torch::TensorOptions().dtype(torch::kDouble)).clone();
    // auto traj_costs = _trajectory_costs.accessor<double,1>();
    if(_maximisation){
        immediate_returns = 1-immediate_returns;
        final_return = 1-final_return;
    }
    if(returns.empty())
        _trajectory_costs[_current_sample] = final_return;
    else
        _trajectory_costs[_current_sample] = final_return + torch::sum(immediate_returns)/immediate_returns.size(0);
    _current_sample++;
}

bool PiBBElite::iterate(){
    // std::cout << "trajectory costs: " << _trajectory_costs << std::endl;
    _elite_costs = torch::cat({_elite_costs,_trajectory_costs});
    std::tuple<torch::Tensor,torch::Tensor> sorted = at::sort(_elite_costs,0);
    // std::cout << "sorted costs: " << std::get<0>(sorted) << " indices: " << std::get<1>(sorted) << std::endl;
    _elite_costs = torch::slice(std::get<0>(sorted),0,0,static_cast<int64_t>(_sample_size*_elite_ratio));
    std::cout << "elite costs: " << _elite_costs << std::endl;
    torch::Tensor min_tc = torch::min(_elite_costs);
    torch::Tensor max_tc = torch::max(_elite_costs);
    torch::Tensor exp_tc = torch::exp(-_inverse_lambda*(_elite_costs-min_tc)/(max_tc - min_tc));
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
    _elite_samples = torch::cat({_elite_samples,_samples});
    // std::cout << "samples: " << _elite_samples << std::endl;
    _elite_samples = torch::index_select(_elite_samples,0,std::get<1>(sorted).slice(0,0,static_cast<int64_t>(_sample_size*_elite_ratio)));
    std::cout << "std diff: " << torch::std(_elite_samples -_policy_params) << std::endl;
    std::cout << "mean doff: " << torch::mean(_elite_samples -_policy_params) << std::endl;

    torch::Tensor delta = torch::sum(torch::mul(torch::transpose(_elite_samples-_policy_params,0,1),_probabilities),1);
    std::cout << "std delta: " << torch::std(delta) << std::endl;
    std::cout << "mean delta: " << torch::mean(delta) << std::endl;
    if(torch::isnan(_probabilities).any().item<bool>()){
        std::cerr << "Error PiBB::iterate: NaN in probabilities" << std::endl;
        return false;
    }
    // std::cout << "delta: " << delta << std::endl;
    _policy_params += delta;
    _policy_params = torch::clamp(_policy_params,-1,1);
    _sigma *= _sigma_decay;
    _inverse_lambda *= _lambda_decay;
    return true;
}

void PiBBElite::generate_samples(torch::Tensor &samples){
    _current_sample = 0;
    for(int i = 0; i < _sample_size; i++){
        for(int j = 0; j < _exp_noise.size(1); j++){
            _exp_noise[i][j] = _rand_num->normalDist(0,_sigma);
        }
    }
    _samples = torch::clamp(_policy_params + _exp_noise,-1,1);
    samples = _samples;
}
