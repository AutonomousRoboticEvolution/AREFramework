#include "mlp.hpp"

using namespace tnn;

MLP::MLP(int nbr_inputs, int nbr_outputs, int nbr_hidden):
_nbr_inputs(nbr_inputs), _nbr_ouputs(nbr_outputs), _nbr_hidden(nbr_hidden)
{
    _in_to_hid = torch::nn::Linear(nbr_inputs,nbr_hidden);
    _hid_to_out = torch::nn::Linear(nbr_hidden,nbr_outputs);
}
torch::Tensor MLP::forward(torch::Tensor x){
    std::cout << x.size(0) << std::endl;
    x = _in_to_hid->forward(x);
    x = torch::sigmoid(x);
    x = torch::sigmoid(_hid_to_out->forward(x));
    return x;            
}

void MLP::set_weights_biases(const std::vector<double> &weights, const std::vector<double> &biases){
    //split the weights vector into two for each layers
    std::vector<double> ih_ws;
    ih_ws.insert(ih_ws.begin(),weights.begin(),weights.begin() + _nbr_inputs*_nbr_hidden);
    std::vector<double> ho_ws;
    ho_ws.insert(ho_ws.begin(),weights.begin() + _nbr_inputs*_nbr_hidden,weights.end());
    //set the weights
    _in_to_hid->weight = torch::from_blob(ih_ws.data(),{static_cast<int64_t>(ih_ws.size())},torch::TensorOptions().dtype(torch::kDouble));
    _hid_to_out->weight = torch::from_blob(ho_ws.data(),{static_cast<int64_t>(ho_ws.size())},torch::TensorOptions().dtype(torch::kDouble));

    //split the biases vector into two for each layers
    std::vector<double> h_bs;
    h_bs.insert(h_bs.begin(),biases.begin(),biases.begin() + _nbr_hidden);
    std::vector<double> o_bs;
    o_bs.insert(o_bs.begin(),biases.begin()+_nbr_hidden,biases.end());
    //set the biases
    _in_to_hid->bias = torch::from_blob(h_bs.data(),{static_cast<int64_t>(h_bs.size())},torch::TensorOptions().dtype(torch::kDouble));
    _hid_to_out->weight = torch::from_blob(o_bs.data(),{static_cast<int64_t>(o_bs.size())},torch::TensorOptions().dtype(torch::kDouble));
}

std::vector<double> MLP::get_weights(){
    std::vector<double> weights;
    for(int i = 0; i < _in_to_hid->weight.size(0); i++)
        weights.push_back(_in_to_hid->weight[i].item<double>());
    for(int i = 0; i < _hid_to_out->weight.size(0); i++)
        weights.push_back(_hid_to_out->weight[i].item<double>());
    return weights;
}

std::vector<double> MLP::get_biases(){
    std::vector<double> biases;
    for(int i = 0; i < _in_to_hid->weight.size(0); i++)
        biases.push_back(_in_to_hid->bias[i].item<double>());
    for(int i = 0; i < _hid_to_out->bias.size(0); i++)
        biases.push_back(_hid_to_out->bias[i].item<double>());
    return biases;
}

using namespace are;

std::vector<double> MLPControl::update(const std::vector<double> &inputs){
    std::vector<double> cpy = inputs;
    torch::Tensor t_in = torch::from_blob(cpy.data(),{static_cast<int64_t>(inputs.size())},torch::TensorOptions().dtype(torch::kDouble));
    torch::Tensor t_out = _nn->forward(t_in);
    std::vector<double> outputs(_nn->_nbr_ouputs);
    for(int i = 0; i < t_out.size(0);i++)
        outputs[i] = t_out[i].item<double>();
    return outputs;
}

void MLPControl::init_nn(int nbr_inputs, int nbr_outputs, int nbr_hidden){
    _nn = std::make_shared<tnn::MLP>(nbr_inputs,nbr_outputs,nbr_hidden);
}

void MLPControl::nbr_parameters(int nbr_inputs, int nbr_outputs, int nbr_hidden,int &nbr_weights, int &nbr_biases){
    nbr_weights = nbr_inputs*nbr_hidden + nbr_hidden*nbr_outputs;
    nbr_biases = nbr_hidden + nbr_outputs;
}
