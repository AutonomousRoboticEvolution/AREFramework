#pragma once 

#include <vector>
#include <torch/torch.h>
#include "ARE/Control.h"

namespace tnn{


typedef enum type_t{
    t_MLP = 0,
    t_RNN = 1,
    t_CPGRBF = 2,
    t_CPGRBFRNN = 3
}type_t;

struct MLP : torch::nn::Module{
    MLP(int nbr_inputs, int nbr_ouputs, int nbr_hidden);

    torch::Tensor forward(torch::Tensor x);

    void set_weights_biases(std::vector<double> weights, std::vector<double> biases);
    void set_weights_biases(const torch::Tensor& weights, const torch::Tensor& biases);
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

    void set_weights_biases(std::vector<double> weights, std::vector<double> biases);
    void set_weights_biases(const torch::Tensor& weights, const torch::Tensor& biases);
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

/**
 * @brief Central Pattern Generator cell implementation based on the SO(2)-Networks Oscillators model from the paper:
 * "SO(2)-Networks as Neural Oscillators", F. Pasemann, 2003, In International work-conference on artificial neural networks
 */
struct CPGCellImpl: torch::nn::Module{
    CPGCellImpl(double init_state_0,double init_state_1);
    torch::Tensor forward();

    void set_weights_biases(std::vector<double> weights, std::vector<double> biases);
    void set_weights_biases(const torch::Tensor& weights, const torch::Tensor& biases);
    const torch::Tensor &neuron_states(){return _neuron_states;}

private:
    torch::Tensor _neuron_states;
    torch::Tensor _weights;
    torch::Tensor _biases;

};

TORCH_MODULE(CPGCell);

/**
 * @brief Implementation of RBF units with a gaussian kernel from the paper
 * "Generic Neural Locomotion Control  Framework for Legged Robots", M. Thor et al., 2021, IEEE TRANSACTIONS ON NEURAL NETWORKS AND LEARNING SYSTEMS
 * The implementation is modified to integrate sensory inputs
 */

struct RBFCellImpl: torch::nn::Module{
    RBFCellImpl(int nbr_inputs,int nbr_rbf);
    torch::Tensor forward(torch::Tensor x);

    void set_centers(std::vector<double> centers);
    void set_centers(const torch::Tensor& centers);
    void set_variances(std::vector<double> variances);
    void set_variances(const torch::Tensor& variances);
    torch::Tensor get_centers(){return _centers;}
    torch::Tensor get_variances(){return _variances;}

    int _nbr_inputs;
    int _nbr_rbf;

private:
    torch::Tensor _centers;
    torch::Tensor _variances;
};

TORCH_MODULE(RBFCell);

struct RBFNetwork: torch::nn::Module{
    RBFNetwork(int nbr_inputs,int nbr_hidden,int nbr_outputs);
    torch::Tensor forward(torch::Tensor x);

    void set_weights_biases(std::vector<double> weights, std::vector<double> biases);
    void set_weights_biases(const torch::Tensor& weights, const torch::Tensor& biases);
    void set_centers(std::vector<double> centers);
    void set_centers(const torch::Tensor& centers);
    void set_variances(std::vector<double> variances);
    void set_variances(const torch::Tensor& variances);

    torch::Tensor get_centers(){return _rbf->get_centers();}
    torch::Tensor get_variances(){return _rbf->get_variances();}


    int _nbr_inputs;
    int _nbr_outputs;
    int _nbr_hidden;

private:
    RBFCell _rbf{nullptr};
    torch::nn::Linear _out{nullptr};
};


/**
 * @brief Implementation of CPGRBF network from the paper
 * "Generic Neural Locomotion Control  Framework for Legged Robots", M. Thor, 2021, IEEE TRANSACTIONS ON NEURAL NETWORKS AND LEARNING SYSTEMS
 * The implementation is modified to integrate sensory inputs
 */
struct CPGRBFNetwork: torch::nn::Module{
    /**
     * @brief CPGRBFNetwork constuctors
     * @param nbr_inputs
     * @param nbr_hidden
     * @param nbr_outputs
     * @param init_state_0 of the CPGCell with default values to get a stable sine wave
     * @param init_state_1 of the CPGCell with default values to get a stable sine wave
     */
    CPGRBFNetwork(int nbr_hidden,int nbr_outputs, double init_state_0 = 0.2, double init_state_1 = -0.2);
    torch::Tensor forward();

    /**
     * @brief init the parameters of the cpg to have a stable sine wave.
     * Parameters from the original paper
     */
    void init_cpg(double alpha, double phi);

    /**
     * @brief init the centers and variance of the rbf layer with parameters from the original paper
     */
    void init_rbf();

    void set_cpg_parameters(std::vector<double> weights, std::vector<double> biases);
    void set_rbf_parameters(std::vector<double> centers,std::vector<double> variances);
    void set_out_layer_parameters(std::vector<double> weights, std::vector<double> biases);
    void set_cpg_parameters(const torch::Tensor& weights, const torch::Tensor& biases);
    void set_rbf_parameters(const torch::Tensor& centers,const torch::Tensor& variances);
    void set_out_layer_parameters(const torch::Tensor& weights, const torch::Tensor& biases);
    // void get_out_layer_parameters(std::vector<double> &weights, std::vector<double> &biases);

    int _nbr_outputs;
    int _nbr_hidden;
private:
    CPGCell _cpg{nullptr};
    RBFCell _rbf{nullptr};
    torch::nn::Linear _out{nullptr};
};

struct CPGRBFRNN: torch::nn::Module{
    /**
     * @brief CPGRBFRNN constuctors
     * @param nbr_inputs
     * @param nbr_rbf
     * @param nbr_rnn
     * @param nbr_outputs
     * @param init_state_0 of the CPGCell with default values to get a stable sine wave
     * @param init_state_1 of the CPGCell with default values to get a stable sine wave
     */
    CPGRBFRNN(int nbr_inputs,int nbr_rbf, int nbr_rnn,int nbr_outputs, 
        double init_state_0 = 0.2, double init_state_1 = -0.2);
    torch::Tensor forward(torch::Tensor x);

    /**
     * @brief init the parameters of the cpg to have a stable sine wave.
     * Parameters from the original paper
     */
    void init_cpg(double alpha, double phi);

    /**
     * @brief init the centers and variance of the rbf layer with parameters from the original paper
     */
    void init_rbf();

    void set_cpg_parameters(std::vector<double> weights, std::vector<double> biases);
    void set_rbf_parameters(std::vector<double> centers,std::vector<double> variances);
    void set_out_layer_parameters(std::vector<double> weights, std::vector<double> biases);
    void set_rnn_parameters(std::vector<double> weights, std::vector<double> biases);
    void set_cpg_parameters(const torch::Tensor& weights, const torch::Tensor& biases);
    void set_rbf_parameters(const torch::Tensor& centers,const torch::Tensor& variances);
    void set_out_layer_parameters(const torch::Tensor& weights, const torch::Tensor& biases);
    void set_rnn_parameters(const torch::Tensor& weights, const torch::Tensor& biases);

    int _nbr_inputs;
    int _nbr_outputs;
    int _nbr_rbf;
    int _nbr_rnn;
private:
    CPGCell _cpg{nullptr};
    RBFCell _rbf{nullptr};
    torch::nn::RNNCell _rnn{nullptr};
    torch::Tensor _rnn_state{nullptr};
    torch::nn::Linear _rnn_to_rbf{nullptr};
    torch::nn::Linear _out{nullptr};
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
    void set_randomNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    std::shared_ptr<tnn::RNN> _nn;
};

class CPGRBFControl: public Control{
public:
    CPGRBFControl() : Control(){}
    CPGRBFControl(int nbr_outputs, int nbr_rbf,
        double init_state_0 = 0.2, double init_state_1 = -0.2,double alpha = 1.05, double phi = 0.1) : Control(){
        init_nn(nbr_outputs,nbr_rbf,init_state_0,init_state_1,alpha,phi);
    }
    Control::Ptr clone() const override{
        return std::make_shared<CPGRBFControl>(*this);
    }

    std::vector<double> update(const std::vector<double> &inputs) override;

    void init_nn(int nbr_outputs, int nbr_rbf, 
        double init_state_0 = 0.2, double init_state_1 = -0.2, double alpha = 1.05, double phi = 0.1);
    static void nbr_parameters(int nbr_outputs,int nbr_rbf,int &nbr_weights, int &nbr_biases);
    void set_randomNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    std::shared_ptr<tnn::CPGRBFNetwork> _nn;
};

class CPGRBFRNNControl: public Control{
public:
    CPGRBFRNNControl() : Control(){}
    CPGRBFRNNControl(int nbr_inputs,int nbr_outputs, int nbr_rbf, int nbr_rnn, 
        double init_state_0 = 0.2, double init_state_1 = -0.2, double alpha = 1.05, double phi = 0.1) : Control(){
        init_nn(nbr_inputs,nbr_outputs,nbr_rbf,nbr_rnn,init_state_0,init_state_1,alpha,phi);
    }
    Control::Ptr clone() const override{
        return std::make_shared<CPGRBFRNNControl>(*this);
    }

    std::vector<double> update(const std::vector<double> &inputs) override;

    void init_nn(int nbr_inputs, int nbr_outputs, int nbr_rbf, int nbr_rnn, 
        double init_state_0 = 0.2, double init_state_1 = -0.2, double alpha = 1.05, double phi = 0.1);
    static void nbr_parameters(int nbr_inputs, int nbr_outputs,int nbr_rbf, int nbr_rnn,int &nbr_rnn_weights,int &nbr_out_weights, int &nbr_rnn_biases, int &nbr_out_biases);
    void set_randomNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    std::shared_ptr<tnn::CPGRBFRNN> _nn;
};

}
