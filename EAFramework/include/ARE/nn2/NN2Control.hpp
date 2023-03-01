#ifndef NN2CONTROL_H
#define NN2CONTROL_H

#include "ARE/Control.h"
#include "NN2Settings.hpp"
#include <eigen3/Eigen/Core>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include "ARE/misc/RandNum.h"
#include "nn2/mlp.hpp"
#include "nn2/elman.hpp"
#include "nn2/rnn.hpp"
#include "nn2/fcp.hpp"
#include "nn2/elman_cpg.hpp"
#include "nn2/cpg.hpp"


namespace are {
namespace control{
using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using connection_t = nn2::Connection<double>;
}
using ffnn_t = nn2::Mlp<control::neuron_t,control::connection_t>;
using elman_t = nn2::Elman<control::neuron_t,control::connection_t>;
using rnn_t = nn2::Rnn<control::neuron_t,control::connection_t>;
using fcp_t = nn2::Fcp<control::neuron_t,control::connection_t>;
using elman_cpg_t = nn2::ElmanCPG<control::neuron_t,control::connection_t>;
using cpg_t = nn2::CPG<control::neuron_t,control::connection_t>;

template<class nn_t>
class NN2Control : public Control
{
public:
    NN2Control() : Control(){
        settings::defaults::parameters->emplace("#UseInternalBias",new settings::Boolean(1));
        settings::defaults::parameters->emplace("#noiseLevel",new settings::Double(0));

    }
    NN2Control(const NN2Control& ctrl) : Control(ctrl){
        settings::defaults::parameters->emplace("#UseInternalBias",new settings::Boolean(1));
        settings::defaults::parameters->emplace("#noiseLevel",new settings::Double(0));
    }

    Control::Ptr clone() const override {
        return std::make_shared<NN2Control>(*this);
    }

    std::vector<double> update(const std::vector<double> &sensorValues){
        double input_noise_lvl = settings::getParameter<settings::Double>(parameters,"#inputNoiseLevel").value;
        double output_noise_lvl = settings::getParameter<settings::Double>(parameters,"#outputNoiseLevel").value;
        float evalTime = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;

        std::vector<double> inputs = sensorValues;
        if(input_noise_lvl > 0.0){
            for(double &sv : inputs){
                sv = randomNum->normalDist(sv,input_noise_lvl);
            }
        }
        nn.step(inputs,evalTime);
        std::vector<double> output = nn.outf();

        if(output_noise_lvl > 0.0){
            for(double &o : output){
                o = randomNum->normalDist(o,output_noise_lvl);;
            }
        }

        return output;
    }


    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    void init_nn(int nb_input, int nb_hidden, int nb_output,const std::vector<double> &weights, const std::vector<double> &biases){
        nn = nn_t(nb_input,nb_hidden,nb_output);
        if(nn.get_nb_connections() != weights.size() || nn.get_nb_neurons() != biases.size()){
            std::cerr << "NN2 init error: Wrong number of parameters" << std::endl;
            return;
        }
        nn.set_all_weights(weights);
        nn.set_all_biases(biases);
        nn.set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
        nn.init();
    }
    void init_nn(int nb_input, int nb_hidden, int nb_output,const std::vector<double> &weights, const std::vector<double> &biases, const std::vector<int> &joint_subs){
        nn = nn_t(nb_input,nb_hidden,nb_output, joint_subs);
        if(nn.get_nb_connections() != weights.size() || nn.get_nb_neurons() != biases.size()){
            std::cerr << "NN2 init error: Wrong number of parameters" << std::endl;
            return;
        }
        nn.set_all_weights(weights);
        nn.set_all_biases(biases);
        nn.set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
        nn.init();
    }

    static void nbr_parameters(int nb_input,int nb_hidden,int nb_output, int &nbr_weights, int &nbr_biases){
        nn_t nn(nb_input,nb_hidden,nb_output);
        nbr_weights = nn.get_nb_connections();
        nbr_biases = nn.get_nb_neurons();
    }

    static void nbr_parameters_cpg(int nb_input,int nb_hidden,int nb_output, int &nbr_weights, int &nbr_biases, const std::vector<int> &joint_subs){
        nn_t nn(nb_input,nb_hidden,nb_output,joint_subs);
        nbr_weights = nn.get_nb_connections();
        nbr_biases = nn.get_nb_neurons();
    }

    nn_t nn;

};

inline void get_nbr_weights_biases(int nbr_inputs,int nbr_outputs, int nbr_hiddens, int nn_type, int &nbr_weights, int &nbr_biases){
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nbr_inputs,nbr_hiddens,nbr_outputs,nbr_weights,nbr_biases);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nbr_inputs,nbr_hiddens,nbr_outputs,nbr_weights,nbr_biases);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nbr_inputs,nbr_hiddens,nbr_outputs,nbr_weights,nbr_biases);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }
}

}//are

#endif //NN2CONTROL_H
