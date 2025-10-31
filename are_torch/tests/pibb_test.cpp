#include <iostream>
#include "are_torch/pi_bb.hpp"
#include "are_torch/pi_bb_elite.hpp"

using namespace are::learning;

int main(int argc, char** argv){
    std::random_device rd;
    std::function<double(const torch::Tensor&)> sphere =
    [](const torch::Tensor& sol) -> double{
        return torch::sum(torch::square(sol)).item<double>();
    };
    PiBBElite pibb;
    pibb.set_rand_num(std::make_shared<are::misc::RandNum>(rd()));
    pibb.init(10,5,0.01,1,1,1,0.5);
    pibb.policy_params() = torch::rand(5);
    std::cout << "initial solution: " << pibb.policy_params() << std::endl;
    for(int iter = 0; iter < 100; iter++){
        torch::Tensor samples;
        pibb.generate_samples(samples);
        for(int k = 0; k < 10; k++){
            // std::cout << "sample: " << samples[k] << std::endl;
            double reward = sphere(samples[k]);
            // std::cout << "reward: " << reward << std::endl;
            pibb.step(are::rollout_t(),{},reward);
        }
        pibb.iterate();
        std::cout << iter << ";" << sphere(pibb.policy_params()) << std::endl;
     }
}
