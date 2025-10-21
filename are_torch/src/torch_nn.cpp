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

void MLP::set_weights_biases(std::vector<double> weights, std::vector<double> biases){
    using namespace torch;
    set_weights_biases(torch::from_blob(weights.data(),
                                        {static_cast<int64_t>(weights.size())},
                                        TensorOptions().dtype(kDouble)),
                       torch::from_blob(biases.data(),
                           {static_cast<int64_t>(biases.size())},
                           TensorOptions().dtype(kDouble)));
}

void MLP::set_weights_biases(const torch::Tensor &weights, const torch::Tensor &biases){
    using namespace torch;

    //set the weights
    _in_to_hid->weight = reshape(slice(weights,0,0,_nbr_inputs*_nbr_hidden),
                                 {_nbr_hidden,_nbr_inputs});
    _hid_to_out->weight = reshape(slice(weights,0,_nbr_inputs*_nbr_hidden,weights.size(0)),
                                  {_nbr_outputs,_nbr_hidden});

    //set the biases
    _in_to_hid->bias = slice(biases,0,0,_nbr_hidden);
    _hid_to_out->bias = slice(biases,0,_nbr_hidden,biases.size(0));;
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
    _rnn = RNNCell(RNNCellOptions(nbr_inputs,nbr_hidden));
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

void RNN::set_weights_biases(std::vector<double> weights, std::vector<double> biases){
    using namespace torch;
    set_weights_biases(torch::from_blob(weights.data(),
                                        {static_cast<int64_t>(weights.size())},
                                        TensorOptions().dtype(kDouble)),
                       torch::from_blob(biases.data(),
                                        {static_cast<int64_t>(biases.size())},
                                        TensorOptions().dtype(kDouble)));
}

void RNN::set_weights_biases(const torch::Tensor &weights, const torch::Tensor &biases){
    using namespace torch;

    //set the weights
    _rnn->weight_ih = reshape(slice(weights,0,0,_nbr_inputs*_nbr_hidden),
                              {_nbr_hidden,_nbr_inputs});

    _rnn->weight_hh = reshape(slice(weights,0,_nbr_inputs*_nbr_hidden,_nbr_inputs*_nbr_hidden + _nbr_hidden*_nbr_hidden),
                              {_nbr_hidden,_nbr_hidden});
    _out->weight = reshape(slice(weights,0,_nbr_inputs*_nbr_hidden + _nbr_hidden*_nbr_hidden,weights.size(0)),
                                  {_nbr_outputs,_nbr_hidden});

    //set the biases
    _rnn->bias_ih = slice(biases,0,0,_nbr_hidden);
    _rnn->bias_hh = slice(biases,0,_nbr_hidden,_nbr_hidden + _nbr_hidden);
    _out->bias = slice(biases,0,_nbr_hidden+_nbr_hidden,biases.size(0));;
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

CPGCellImpl::CPGCellImpl(double init_state_0,double init_state_1){
    _neuron_states = torch::tensor({init_state_0,init_state_1},
                                   torch::TensorOptions().dtype(torch::kDouble));
    _weights = torch::empty({2,2},
                            torch::TensorOptions().dtype(torch::kDouble));
    _biases = torch::empty(2,torch::TensorOptions().dtype(torch::kDouble));
}

torch::Tensor CPGCellImpl::forward(){
    _neuron_states = torch::matmul(_weights,torch::tanh(_neuron_states)) + _biases;
    return _neuron_states;
}

void CPGCellImpl::set_weights_biases(std::vector<double> weights, std::vector<double> biases){
    using namespace torch;
    set_weights_biases(torch::from_blob(weights.data(),
                                        {static_cast<int64_t>(weights.size())},
                                        TensorOptions().dtype(kDouble)).clone(),
                       torch::from_blob(biases.data(),
                                        {static_cast<int64_t>(biases.size())},
                                        TensorOptions().dtype(kDouble)).clone());
}

void CPGCellImpl::set_weights_biases(const torch::Tensor& weights, const torch::Tensor& biases){
    using namespace torch;
    _weights = reshape(weights.clone(),{2,2});
    _biases = biases.clone();

}

RBFCellImpl::RBFCellImpl(int nbr_inputs,int nbr_rbf)
    :_nbr_inputs(nbr_inputs), _nbr_rbf(nbr_rbf)
{
    using namespace torch;
    using namespace torch::nn;
    _centers = torch::empty({nbr_inputs,nbr_rbf},TensorOptions().dtype(kDouble));
    _variances = torch::empty(nbr_rbf,TensorOptions().dtype(kDouble));
}

torch::Tensor RBFCellImpl::forward(torch::Tensor x){
    //RBF hidden layer with gaussian kernel
    x = torch::sub(x,_centers);
    x = torch::pow(x,2);
    x = torch::sum(x,1);
    x = torch::div(x,torch::pow(_variances,2));
    x = torch::exp(torch::neg(x)); 
    return x;
}



void RBFCellImpl::set_centers(std::vector<double> centers){
    using namespace torch;
    set_centers(torch::from_blob(
            centers.data(),
                {static_cast<int64_t>(centers.size())},
                TensorOptions().dtype(kDouble)
            ));
}
void RBFCellImpl::set_variances(std::vector<double> variances){
    using namespace torch;
    set_variances(torch::from_blob(
            variances.data(),
            {static_cast<int64_t>(variances.size())},
            TensorOptions().dtype(kDouble)
            ));
}
void RBFCellImpl::set_centers(const torch::Tensor& centers){
    using namespace torch;
    _centers = reshape(centers.clone(),{_nbr_rbf,_nbr_inputs});
}
void RBFCellImpl::set_variances(const torch::Tensor& variances){
    using namespace torch;
    _variances = variances.clone();
}

RBFNetwork::RBFNetwork(int nbr_inputs,int nbr_hidden,int nbr_outputs)
    :_nbr_inputs(nbr_inputs), _nbr_outputs(nbr_outputs), _nbr_hidden(nbr_hidden),
    _rbf(nbr_inputs,nbr_hidden), _out(nbr_hidden,nbr_outputs)
{
    _rbf->to(torch::kDouble);
    _out->to(torch::kDouble);
}

torch::Tensor RBFNetwork::forward(torch::Tensor x){
    x = _rbf->forward(x);
    x = _out->forward(x);
    return x;
}
void RBFNetwork::set_centers(std::vector<double> centers){
    _rbf->set_centers(centers);
}
void RBFNetwork::set_variances(std::vector<double> variances){
    _rbf->set_variances(variances);
}
void RBFNetwork::set_centers(const torch::Tensor& centers){
    _rbf->set_centers(centers);
}
void RBFNetwork::set_variances(const torch::Tensor& variances){
    _rbf->set_variances(variances);
}
void RBFNetwork::set_weights_biases(std::vector<double> weights, std::vector<double> biases){
    using namespace torch;
    set_weights_biases(torch::from_blob(
                weights.data(),
                {static_cast<int64_t>(weights.size())},
                TensorOptions().dtype(kDouble)),
                torch::from_blob(
                  biases.data(),
                  {static_cast<int64_t>(biases.size())},
                  TensorOptions().dtype(kDouble)));
}
void RBFNetwork::set_weights_biases(const torch::Tensor& weights, const torch::Tensor& biases){
    using namespace torch;
    _out->weight = reshape(weights.clone(),{_nbr_outputs,_nbr_hidden});
    _out->bias = biases.clone();
}

CPGRBFNetwork::CPGRBFNetwork(int nbr_hidden,int nbr_outputs,  double init_state_0, double init_state_1)
    : _nbr_outputs(nbr_outputs), _nbr_hidden(nbr_hidden),
    _cpg(init_state_0,init_state_1), _rbf(2,nbr_hidden), _out(nbr_hidden, _nbr_outputs)
{
    _out->to(torch::kDouble);
    _cpg->to(torch::kDouble);
    _rbf->to(torch::kDouble);
}

torch::Tensor CPGRBFNetwork::forward(){
    torch::Tensor x = _cpg->forward();
    x = _rbf->forward(x);
    x = _out->forward(x);
    return torch::tanh(x);
}

void CPGRBFNetwork::init_cpg(double alpha, double phi){
    _cpg->set_weights_biases({alpha*cos(phi*M_PI),alpha*sin(phi*M_PI),
                              -alpha*sin(phi*M_PI),alpha*cos(phi*M_PI)},
                             {0,0});
}
void CPGRBFNetwork::init_rbf(){
    torch::Tensor x;
    std::vector<double> centers;
    for(int i = 0;i < _nbr_hidden; i++ ){
        x = _cpg->forward();
        centers.insert(centers.end(),x.data_ptr<double>(),x.data_ptr<double>()+2);
    }
    _rbf->set_centers(centers);
    _rbf->set_variances(std::vector<double>(_nbr_hidden,0.2));
}

void CPGRBFNetwork::set_cpg_parameters(std::vector<double> weights, std::vector<double> biases){
    _cpg->set_weights_biases(weights,biases);
}
void CPGRBFNetwork::set_rbf_parameters(std::vector<double> centers,std::vector<double> variances){
    _rbf->set_centers(centers);
    _rbf->set_variances(variances);
}
void CPGRBFNetwork::set_out_layer_parameters(std::vector<double> weights, std::vector<double> biases){
    using namespace torch;
    set_out_layer_parameters(torch::from_blob(
                weights.data(),
                {static_cast<int64_t>(weights.size())},
                TensorOptions().dtype(kDouble)),
                torch::from_blob(
                biases.data(),
                {static_cast<int64_t>(biases.size())},
                TensorOptions().dtype(kDouble)));
}

void CPGRBFNetwork::set_cpg_parameters(const torch::Tensor& weights, const torch::Tensor& biases){
    _cpg->set_weights_biases(weights,biases);
}
void CPGRBFNetwork::set_rbf_parameters(const torch::Tensor& centers,const torch::Tensor& variances){
    _rbf->set_centers(centers);
    _rbf->set_variances(variances);
}
void CPGRBFNetwork::set_out_layer_parameters(const torch::Tensor& weights, const torch::Tensor& biases){
    using namespace torch;
    _out->weight = reshape(weights.clone(),{_nbr_outputs,_nbr_hidden});
    _out->bias = biases.clone();
}

CPGRBFRNN::CPGRBFRNN(int nbr_inputs,int nbr_rbf, int nbr_rnn,int nbr_outputs, double init_state_0, double init_state_1)
    : _nbr_inputs(nbr_inputs), _nbr_outputs(nbr_outputs), _nbr_rbf(nbr_rbf), _nbr_rnn(nbr_rnn), 
    _cpg(init_state_0,init_state_1), _rbf(2,nbr_rbf), _rnn(nbr_inputs,nbr_rnn), _out(nbr_rbf,nbr_outputs)
{
    using namespace torch;
    using namespace torch::nn;
    _rnn_to_rbf = Linear(LinearOptions(nbr_rnn,nbr_rbf).bias(false));
    _rnn_to_rbf->to(kDouble);
    _out->to(kDouble);
    _rnn->to(kDouble);
    _rnn_to_rbf->weight =
    reshape(
        torch::from_blob(
            std::vector<double>(nbr_rnn*nbr_rbf,1).data(),
            {static_cast<int64_t>(nbr_rnn*nbr_rbf)},
            TensorOptions().dtype(kDouble)).clone(),
        {_nbr_rbf,_nbr_rnn});
    _rnn_state = torch::zeros(_nbr_rnn,TensorOptions().dtype(kDouble));

}

torch::Tensor CPGRBFRNN::forward(torch::Tensor x){
    torch::Tensor cpg_out = _cpg->forward();
    torch::Tensor rbf_state =  _rbf->forward(cpg_out);
    _rnn_state = _rnn->forward(x,_rnn_state);
    torch::Tensor sensory_weights = _rnn_to_rbf->forward(_rnn_state);
    x = torch::mul(rbf_state,sensory_weights);
    x = _out->forward(x);
    return torch::tanh(x);
}

void CPGRBFRNN::init_cpg(double alpha, double phi){
    _cpg->set_weights_biases({alpha*cos(phi*M_PI),alpha*sin(phi*M_PI),
                              -alpha*sin(phi*M_PI),alpha*cos(phi*M_PI)},
                             {0,0});
}
void CPGRBFRNN::init_rbf(){
    torch::Tensor x;
    std::vector<double> centers;
    for(int i = 0;i < _nbr_rbf; i++ ){
        x = _cpg->forward();
        centers.insert(centers.end(),x.data_ptr<double>(),x.data_ptr<double>()+2);
    }
    _rbf->set_centers(centers);
    _rbf->set_variances(std::vector<double>(_nbr_rbf,0.2));
}

void CPGRBFRNN::set_cpg_parameters(std::vector<double> weights, std::vector<double> biases){
    _cpg->set_weights_biases(weights,biases);
}
void CPGRBFRNN::set_rbf_parameters(std::vector<double> centers,std::vector<double> variances){
    _rbf->set_centers(centers);
    _rbf->set_variances(variances);
}
void CPGRBFRNN::set_rnn_parameters(std::vector<double> weights, std::vector<double> biases){
    using namespace torch;
    //set the weights
    set_rnn_parameters(torch::from_blob(weights.data(),
                             {static_cast<int64_t>(weights.size())},
                             TensorOptions().dtype(kDouble)),
                       torch::from_blob(
            biases.data(),
            {static_cast<int64_t>(biases.size())},
            TensorOptions().dtype(kDouble)));
}
void CPGRBFRNN::set_out_layer_parameters(std::vector<double> weights, std::vector<double> biases){
    using namespace torch;
    set_out_layer_parameters(torch::from_blob(
                weights.data(),
                {static_cast<int64_t>(weights.size())},
                TensorOptions().dtype(kDouble)),
                torch::from_blob(
                  biases.data(),
                  {static_cast<int64_t>(biases.size())},
                  TensorOptions().dtype(kDouble)));
}

void CPGRBFRNN::set_cpg_parameters(const torch::Tensor& weights, const torch::Tensor& biases){
    _cpg->set_weights_biases(weights,biases);
}
void CPGRBFRNN::set_rbf_parameters(const torch::Tensor& centers,const torch::Tensor& variances){
    _rbf->set_centers(centers);
    _rbf->set_variances(variances);
}
void CPGRBFRNN::set_rnn_parameters(const torch::Tensor& weights, const torch::Tensor& biases){
    using namespace torch;
    //set the weights
    _rnn->weight_ih = reshape(slice(weights,0,0,_nbr_inputs*_nbr_rnn),{_nbr_rnn,_nbr_inputs});
    _rnn->weight_hh = reshape(slice(weights,0,_nbr_inputs*_nbr_rnn,weights.size(0)),{_nbr_rnn,_nbr_rnn});

    //set the biases
    _rnn->bias_ih = slice(biases,0,0,_nbr_rnn);
    _rnn->bias_hh = slice(biases,0,_nbr_rnn,biases.size(0));
}
void CPGRBFRNN::set_out_layer_parameters(const torch::Tensor& weights, const torch::Tensor& biases){
    using namespace torch;
    _out->weight = reshape(weights.clone(),{_nbr_outputs,_nbr_rbf});
    _out->bias = biases.clone();
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

std::vector<double> CPGRBFControl::update(const std::vector<double> &inputs){
    torch::Tensor t_out = _nn->forward();
    std::vector<double> outputs(t_out.data_ptr<double>(),t_out.data_ptr<double>() + t_out.numel());
    return outputs;
}

void CPGRBFControl::init_nn(int nbr_outputs, int nbr_rbf, double init_state_0, double init_state_1, double alpha, double phi){
    _nn = std::make_shared<tnn::CPGRBFNetwork>(nbr_rbf, nbr_outputs, init_state_0, init_state_1);
    _nn->init_cpg(alpha,phi);
    _nn->init_rbf();
}

void CPGRBFControl::nbr_parameters(int nbr_outputs,int nbr_rbf,int &nbr_weights, int &nbr_biases){
    nbr_weights = nbr_rbf*nbr_outputs;
    nbr_biases = nbr_outputs;
}

std::vector<double> CPGRBFRNNControl::update(const std::vector<double> &inputs){
    std::vector<double> cpy = inputs;
    torch::Tensor t_in = torch::from_blob(cpy.data(),{static_cast<int64_t>(inputs.size())},torch::TensorOptions().dtype(torch::kDouble));
    torch::Tensor t_out = _nn->forward(t_in);
    std::vector<double> outputs(t_out.data_ptr<double>(),t_out.data_ptr<double>() + t_out.numel());
    return outputs;
}

void CPGRBFRNNControl::init_nn(int nbr_inputs,int nbr_outputs, int nbr_rbf, int nbr_rnn, double init_state_0, double init_state_1, double alpha, double phi){
    _nn = std::make_shared<tnn::CPGRBFRNN>(nbr_inputs,nbr_rbf, nbr_rnn, nbr_outputs, init_state_0, init_state_1);
    _nn->init_cpg(alpha, phi);
    _nn->init_rbf();
}

void CPGRBFRNNControl::nbr_parameters(int nbr_inputs, int nbr_outputs,int nbr_rbf, int nbr_rnn,int &nbr_rnn_weights,int &nbr_out_weights, int &nbr_rnn_biases, int &nbr_out_biases){
    nbr_rnn_weights = nbr_inputs*nbr_rnn + nbr_rnn*nbr_rnn;
    nbr_out_weights = nbr_rbf*nbr_outputs;
    nbr_rnn_biases = nbr_rnn*2;
    nbr_out_biases = nbr_outputs;
}
