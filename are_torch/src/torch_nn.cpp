#include "are_torch/torch_nn.hpp"

using namespace tnn;

MLP::MLP(int nbr_inputs, int nbr_outputs, int nbr_hidden):
    _nbr_inputs(nbr_inputs), _nbr_outputs(nbr_outputs), _nbr_hidden(nbr_hidden)
{
    using namespace torch::nn;
    _in_to_hid = Linear(LinearOptions(nbr_inputs,nbr_hidden));
    _hid_to_out = Linear(LinearOptions(nbr_hidden,nbr_outputs));
    _nn = Sequential(
        _in_to_hid,
        Sigmoid(),
        _hid_to_out,
        Sigmoid()
        );
    _nn->to(torch::kDouble);
}
torch::Tensor MLP::forward(torch::Tensor x){
    x = _nn->forward(x);
    return x;
}

void MLP::print_nn_structure(){
    for(const auto &module: _nn->modules())
        std::cout << module->name() << std::endl;
}

void MLP::set_weights_biases(const std::vector<double> &weights, const std::vector<double> &biases){
    //split the weights vector into two for each layers
    std::vector<double> ih_ws(weights.begin(),weights.begin() + _nbr_inputs*_nbr_hidden);
    std::vector<double> ho_ws(weights.begin() + _nbr_inputs*_nbr_hidden,weights.end());
    //set the weights
    _in_to_hid->weight =
        torch::reshape(
            torch::from_blob(ih_ws.data(),
                         {static_cast<int64_t>(ih_ws.size())},
                         torch::TensorOptions().dtype(torch::kDouble)).clone()
            ,{_nbr_hidden,_nbr_inputs});
    _hid_to_out->weight =
        torch::reshape(
            torch::from_blob(ho_ws.data(),
                         {static_cast<int64_t>(ho_ws.size())},
                         torch::TensorOptions().dtype(torch::kDouble)).clone()
            ,{_nbr_outputs,_nbr_hidden});

    //split the biases vector into two for each layers
    std::vector<double> h_bs(biases.begin(),biases.begin() + _nbr_hidden);
    std::vector<double> o_bs(biases.begin()+_nbr_hidden,biases.end());

    //set the biases
    _in_to_hid->bias =
        torch::from_blob(
            h_bs.data(),
            {static_cast<int64_t>(h_bs.size())},
            torch::TensorOptions().dtype(torch::kDouble)).clone();
    _hid_to_out->bias =
        torch::from_blob(
            o_bs.data(),
            {static_cast<int64_t>(o_bs.size())},
            torch::TensorOptions().dtype(torch::kDouble)).clone();
}

std::vector<double> MLP::get_weights(){
    // torch::Tensor ih_w(_in_to_hid->weight.contiguous());
    double* w_data = _in_to_hid->weight.data_ptr<double>();
    std::vector<double> weights(w_data,w_data+_nbr_inputs*_nbr_hidden);
    w_data = _hid_to_out->weight.data_ptr<double>();
    weights.insert(weights.end(),w_data,w_data+_nbr_outputs*_nbr_hidden);
    return weights;
}

std::vector<double> MLP::get_biases(){

    double* b_data = _in_to_hid->bias.data_ptr<double>();
    std::vector<double> biases(b_data,b_data+_nbr_hidden);
    b_data = _hid_to_out->bias.data_ptr<double>();
    biases.insert(biases.end(),b_data,b_data+_nbr_outputs);
    return biases;
}

RNN::RNN(int nbr_inputs, int nbr_outputs, int nbr_hidden):
    _nbr_inputs(nbr_inputs), _nbr_outputs(nbr_outputs), _nbr_hidden(nbr_hidden)
{
    using namespace torch;
    using namespace torch::nn;
    _rnn = torch::nn::RNNCell(RNNCellOptions(nbr_inputs,nbr_hidden));
    _rnn->to(kDouble);
    _out = Linear(LinearOptions(nbr_hidden,nbr_outputs));
    _out->to(kDouble);
    _hidden_state = torch::zeros(_nbr_hidden,TensorOptions().dtype(kDouble));

}
torch::Tensor RNN::forward(torch::Tensor x){
    _hidden_state = _rnn->forward(x,_hidden_state);
    x = _out->forward(_hidden_state);
    x = torch::tanh(x);
    return x;
}

void RNN::set_weights_biases(const std::vector<double> &weights, const std::vector<double> &biases){
    //split the weights vector into two for each layers
    std::vector<double> ih_ws(weights.begin(),weights.begin() + _nbr_inputs*_nbr_hidden);
    std::vector<double> hh_ws(weights.begin() + _nbr_inputs*_nbr_hidden,weights.begin() +  _nbr_inputs*_nbr_hidden + _nbr_hidden*_nbr_hidden);
    std::vector<double> ho_ws(weights.begin() + _nbr_inputs*_nbr_hidden + _nbr_hidden*_nbr_hidden,weights.end());
    //set the weights
    _rnn->weight_ih =
        torch::reshape(
            torch::from_blob(ih_ws.data(),
                             {static_cast<int64_t>(ih_ws.size())},
                             torch::TensorOptions().dtype(torch::kDouble)).clone()
            ,{_nbr_hidden,_nbr_inputs});
    _rnn->weight_hh =
        torch::reshape(
            torch::from_blob(hh_ws.data(),
                             {static_cast<int64_t>(hh_ws.size())},
                             torch::TensorOptions().dtype(torch::kDouble)).clone()
            ,{_nbr_hidden,_nbr_hidden});

    _out->weight =
        torch::reshape(
            torch::from_blob(ho_ws.data(),
                         {static_cast<int64_t>(ho_ws.size())},
                         torch::TensorOptions().dtype(torch::kDouble)).clone()
        ,{_nbr_outputs,_nbr_hidden});

    //split the biases vector into two for each layers
    std::vector<double> ih_bs(biases.begin(),biases.begin() + _nbr_hidden);
    std::vector<double> hh_bs(biases.begin() + _nbr_hidden,biases.begin() + _nbr_hidden+ _nbr_hidden);
    std::vector<double> o_bs(biases.begin()+_nbr_hidden*2,biases.end());

    //set the biases
    _rnn->bias_ih =
        torch::from_blob(
            ih_bs.data(),
            {static_cast<int64_t>(ih_bs.size())},
            torch::TensorOptions().dtype(torch::kDouble)).clone();
    _rnn->bias_hh =
        torch::from_blob(
            hh_bs.data(),
            {static_cast<int64_t>(hh_bs.size())},
            torch::TensorOptions().dtype(torch::kDouble)).clone();
    _out->bias =
        torch::from_blob(
            o_bs.data(),
            {static_cast<int64_t>(o_bs.size())},
            torch::TensorOptions().dtype(torch::kDouble)).clone();
}

std::vector<double> RNN::get_weights(){
    double* w_data = _rnn->weight_ih.data_ptr<double>();
    std::vector<double> weights(w_data,w_data+_nbr_inputs*_nbr_hidden);
    w_data = _rnn->weight_hh.data_ptr<double>();
    weights.insert(weights.end(),w_data,w_data+_nbr_hidden*_nbr_hidden);
    w_data = _out->weight.data_ptr<double>();
    weights.insert(weights.end(),w_data,w_data+_nbr_outputs*_nbr_hidden);
    return weights;
}

std::vector<double> RNN::get_biases(){
    double* b_data = _rnn->bias_ih.data_ptr<double>();
    std::vector<double> biases(b_data,b_data+_nbr_hidden);
    b_data = _rnn->bias_hh.data_ptr<double>();
    biases.insert(biases.end(),b_data,b_data+_nbr_hidden);
    b_data = _out->bias.data_ptr<double>();
    biases.insert(biases.end(),b_data,b_data+_nbr_outputs);
    return biases;
}

using namespace are;

std::vector<double> MLPControl::update(const std::vector<double> &inputs){
    std::vector<double> cpy = inputs;
    torch::Tensor t_in = torch::from_blob(cpy.data(),{static_cast<int64_t>(inputs.size())},torch::TensorOptions().dtype(torch::kDouble));
    torch::Tensor t_out = _nn->forward(t_in);
    std::vector<double> outputs(t_out.data_ptr<double>(),t_out.data_ptr<double>() + t_out.numel());
    return outputs;
}

void MLPControl::init_nn(int nbr_inputs, int nbr_outputs, int nbr_hidden){
    _nn = std::make_shared<tnn::MLP>(nbr_inputs,nbr_outputs,nbr_hidden);
}

void MLPControl::nbr_parameters(int nbr_inputs, int nbr_outputs, int nbr_hidden,int &nbr_weights, int &nbr_biases){
    nbr_weights = nbr_inputs*nbr_hidden + nbr_hidden*nbr_outputs;
    nbr_biases = nbr_hidden + nbr_outputs;
}

std::vector<double> RNNControl::update(const std::vector<double> &inputs){
    std::vector<double> out_weights =  _nn->get_weights();
    std::vector<double> cpy = inputs;
    torch::Tensor t_in = torch::from_blob(cpy.data(),{static_cast<int64_t>(inputs.size())},torch::TensorOptions().dtype(torch::kDouble));
    torch::Tensor t_out = _nn->forward(t_in);
    std::vector<double> outputs(t_out.data_ptr<double>(),t_out.data_ptr<double>() + t_out.numel());
    return outputs;
}

void RNNControl::init_nn(int nbr_inputs, int nbr_outputs, int nbr_hidden){
    _nn = std::make_shared<tnn::RNN>(nbr_inputs,nbr_outputs,nbr_hidden);
}

void RNNControl::nbr_parameters(int nbr_inputs, int nbr_outputs, int nbr_hidden,int &nbr_weights, int &nbr_biases){
    nbr_weights = nbr_inputs*nbr_hidden + nbr_hidden*nbr_hidden + nbr_hidden*nbr_outputs;
    nbr_biases = nbr_hidden*2 + nbr_outputs;
}
