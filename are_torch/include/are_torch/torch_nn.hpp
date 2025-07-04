#ifndef MLP_HPP
#define MLP_HPP
#include <vector>
#include <torch/torch.h>
#include "ARE/Control.h"

namespace tnn{


typedef enum type_t{
    t_MLP = 0,
    t_RNN = 1
}type_t;

struct MLP : torch::nn::Module{
    MLP(int nbr_inputs, int nbr_ouputs, int nbr_hidden);

    torch::Tensor forward(torch::Tensor x);

    void set_weights_biases(const std::vector<double> &weights, const std::vector<double> &biases);
    std::vector<double> get_weights();
    std::vector<double> get_biases();
    void print_nn_structure();

    int _nbr_inputs;
    int _nbr_outputs;
    int _nbr_hidden;
private:
    torch::nn::Linear _in_to_hid{nullptr};
    torch::nn::Linear _hid_to_out{nullptr};
    torch::nn::Sequential _nn{nullptr};
};


struct RNN : torch::nn::Module{

    RNN(int nbr_inputs, int nbr_outputs, int nbr_hidden);
    torch::Tensor forward(torch::Tensor x);

    void set_weights_biases(const std::vector<double> &weights, const std::vector<double> &biases);
    std::vector<double> get_weights();
    std::vector<double> get_biases();

    int _nbr_inputs;
    int _nbr_outputs;
    int _nbr_hidden;

private:
    torch::nn::RNNCell _rnn{nullptr};
    torch::nn::Linear _out{nullptr};
    torch::nn::Sequential _nn{nullptr};
    torch::Tensor _hidden_state;
};

}//tnn

namespace are{
class MLPControl: public Control{
public:
    MLPControl() : Control(){}
    MLPControl(int nbr_inputs, int nbr_outputs, int nbr_hidden) : Control(){
        init_nn(nbr_inputs,nbr_outputs,nbr_hidden);
    }
    Control::Ptr clone() const override{
        return std::make_shared<MLPControl>(*this);
    }

    std::vector<double> update(const std::vector<double> &inputs) override;

    void init_nn(int nbr_inputs, int nbr_outputs, int nbr_hidden);
    static void nbr_parameters(int nbr_inputs, int nbr_ouputs, int nbr_hidden,int &nbr_weights, int &nbr_biases);
    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    std::shared_ptr<tnn::MLP> _nn;
};

class RNNControl: public Control{
public:
    RNNControl() : Control(){}
    RNNControl(int nbr_inputs, int nbr_outputs, int nbr_hidden) : Control(){
        init_nn(nbr_inputs,nbr_outputs,nbr_hidden);
    }
    Control::Ptr clone() const override{
        return std::make_shared<RNNControl>(*this);
    }

    std::vector<double> update(const std::vector<double> &inputs) override;

    void init_nn(int nbr_inputs, int nbr_outputs, int nbr_hidden);
    static void nbr_parameters(int nbr_inputs, int nbr_ouputs, int nbr_hidden,int &nbr_weights, int &nbr_biases);
    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    std::shared_ptr<tnn::RNN> _nn;
};

}

#endif //MLP_HPP
