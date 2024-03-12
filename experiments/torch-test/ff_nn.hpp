#ifndef FF_NN_HPP
#define FF_NN_HPP

#include <torch/torch.h>
#include <iostream>



class FeedForwardNN: public torch::nn::Module
{
public:
    FeedForwardNN() :
        _linear1(register_module("linear1",torch::nn::Linear(2,5))),
//        _linear2(register_module("linear2",torch::nn::Linear(5,3))),
        _activation(register_module("activation",torch::nn::Sigmoid()))
    {
        _linear1->weight = torch::ones({5,2});
        std::cout << _linear1->weight << std::endl;
        _linear1->bias = torch::ones({5});
        std::cout << _linear1->bias << std::endl;



    }

    at::Tensor forward(at::Tensor x);


//tensors
    torch::nn::Linear _linear1;
    //torch::nn::Linear _linear2;
    torch::nn::Sigmoid _activation;
};

#endif //FF_NN_HPP
