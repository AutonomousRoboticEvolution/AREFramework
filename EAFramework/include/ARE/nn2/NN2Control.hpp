#ifndef NN2CONTROL_H
#define NN2CONTROL_H

#include "ARE/Control.h"
#include "NN2Settings.hpp"
#include <eigen3/Eigen/Core>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include "nn2/mlp.hpp"
#include "nn2/elman.hpp"
#include "nn2/rnn.hpp"

namespace are {

using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using connection_t = nn2::Connection<double>;
using ffnn_t = nn2::Mlp<neuron_t,connection_t>;
using elman_t = nn2::Elman<neuron_t,connection_t>;
using rnn_t = nn2::Rnn<neuron_t,connection_t>;

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
        double noiselvl = settings::getParameter<settings::Double>(parameters,"#noiseLevel").value;
        boost::mt19937 rng(randomNum->getSeed());
        std::vector<double> inputs = sensorValues;
        if(noiselvl > 0.0){
            for(double &sv : inputs){
                boost::normal_distribution<> normal(sv,noiselvl);
                sv = normal(rng);
            }
        }
        nn.step(inputs);
        std::vector<double> output = nn.outf();

        if(noiselvl > 0.0){
            for(double &o : output){
                boost::normal_distribution<> normal(o,noiselvl);
                o = normal(rng);
            }
        }

        return output;
    }


    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    void init_nn(int nb_input, int nb_hidden, int nb_output,std::vector<double> weights, std::vector<double> biases){
        nn = nn_t(nb_input,nb_hidden,nb_output);
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

}

#endif //NN2CONTROL_H
