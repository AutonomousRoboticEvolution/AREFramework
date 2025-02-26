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
#include "nn2/ff_cpg.hpp"

namespace are {
namespace control{
using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using cpg_neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::Af_cpg<>>;
using connection_t = nn2::Connection<double>;
}
using ffnn_t = nn2::Mlp<control::neuron_t,control::connection_t>;
using elman_t = nn2::Elman<control::neuron_t,control::connection_t>;
using rnn_t = nn2::Rnn<control::neuron_t,control::connection_t>;
using fcp_t = nn2::Fcp<control::neuron_t,control::connection_t>;
using elman_cpg_t = nn2::ElmanCPG<control::neuron_t,control::connection_t>;
using cpg_t = nn2::CPG<control::cpg_neuron_t,control::connection_t>;
using ff_cpg_t = nn2::Ffcpg<nn2::ffcpg::neuron_t,nn2::ffcpg::connection_t>;

template<class nn_t>
class NN2Control : public Control
{
public:
    NN2Control() : Control(){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<const settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<const settings::Double>(0));

    }
    NN2Control(const NN2Control& ctrl) : Control(ctrl), _nn(ctrl._nn){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<const settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<const settings::Double>(0));
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
        _nn->step(inputs,evalTime);
        std::vector<double> output = _nn->outf();

        if(output_noise_lvl > 0.0){
            for(double &o : output){
                o = randomNum->normalDist(o,output_noise_lvl);;
            }
        }

        return output;
    }


    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    void init_nn(int nb_input, int nb_hidden, int nb_output,const std::vector<double> &weights, const std::vector<double> &biases){
        _nn = std::make_shared<nn_t>(nb_input,nb_hidden,nb_output);
        if(_nn->get_nb_connections() != weights.size() || _nn->get_nb_neurons() != biases.size()){
            std::cerr << "NN2 init error: Wrong number of parameters" << std::endl;
            std::cerr << "nb inputs: " << nb_input << " - nb outputs: " << nb_output << " - nb hidden: " << nb_hidden << std::endl;
            std::cerr << "weights: " << weights.size() << " - nb of connections: " << _nn->get_nb_connections() << " - biases: " << biases.size() << " - nb of neurons: " << _nn->get_nb_neurons() << std::endl;
            return;
        }
        _nn->set_all_weights(weights);
        _nn->set_all_biases(biases);
        _nn->set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
        _nn->init();
    }

    void init_nn(int nb_input, int nb_hidden, int nb_output,const std::vector<double> &weights, const std::vector<double> &biases, const std::vector<int> &joint_subs){
        _nn = std::make_shared<nn_t>(nb_input,nb_hidden,nb_output, joint_subs);
        if(_nn->get_nb_connections() != weights.size() || _nn->get_nb_neurons() != biases.size()){
            std::cerr << "NN2 init error: Wrong number of parameters" << std::endl;
            std::cerr << "nb inputs: " << nb_input << " - nb outputs: " << nb_output << " - nb hidden: " << nb_hidden << std::endl;
            std::cerr << "weights: " << weights.size() << " - nb of connections: " << _nn->get_nb_connections() << " - biases: " << biases.size() << " - nb of neurons: " << _nn->get_nb_neurons() << std::endl;
            std::cerr << "joint substrate: ";
            for(int i: joint_subs)
                std::cerr << i << ";";
            std::cerr << std::endl;
            return;
        }
        _nn->set_all_weights(weights);
        _nn->set_all_biases(biases);
        _nn->set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
        _nn->init();
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

    std::shared_ptr<nn_t> _nn;

};

template<>
class NN2Control<cpg_t> : public Control
{
public:
    NN2Control<cpg_t>() : Control(){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<settings::Double>(0));

    }
    NN2Control<cpg_t>(const NN2Control& ctrl) : Control(ctrl), _nn(ctrl._nn){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<settings::Double>(0));
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
        _nn->step(inputs,evalTime);
        std::vector<double> output = _nn->outf();

        if(output_noise_lvl > 0.0){
            for(double &o : output){
                o = randomNum->normalDist(o,output_noise_lvl);;
            }
        }

        return output;
    }


    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    void init_nn(int nb_input, int nb_hidden, int nb_output,const std::vector<double> &weights, const std::vector<double> &biases, const std::vector<int> &joint_subs){
        _nn = std::make_shared<cpg_t>(nb_input,nb_hidden,nb_output, joint_subs);
        if(_nn->get_nb_connections() != weights.size() || _nn->get_nb_neurons() != biases.size()){
            std::cerr << "NN2 init error: Wrong number of parameters" << std::endl;
            std::cerr << "nb inputs: " << nb_input << " - nb outputs: " << nb_output << " - nb hidden: " << nb_hidden << std::endl;
            std::cerr << "weights: " << weights.size() << " - nb of connections: " << _nn->get_nb_connections() << " - biases: " << biases.size() << " - nb of neurons: " << _nn->get_nb_neurons() << std::endl;
            std::cerr << "joint substrate: ";
            for(int i: joint_subs)
                std::cerr << i << ";";
            std::cerr << std::endl;
            return;
        }
        _nn->set_all_weights(weights);
        _nn->set_all_biases(biases);
        _nn->init();
    }

    static void nbr_parameters_cpg(int nb_input,int nb_hidden,int nb_output, int &nbr_weights, int &nbr_biases, const std::vector<int> &joint_subs){
        cpg_t nn(nb_input,nb_hidden,nb_output,joint_subs);
        nbr_weights = nn.get_nb_connections();
        nbr_biases = nn.get_nb_neurons();
    }
    std::shared_ptr<cpg_t> _nn;

};

template<>
class NN2Control<ff_cpg_t> : public Control
{
public:
    NN2Control<ff_cpg_t>() : Control(){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<const settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<settings::Double>(0));

    }
    NN2Control<ff_cpg_t>(const NN2Control& ctrl) : Control(ctrl), _nn(ctrl._nn){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<settings::Double>(0));
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
        _nn->step(inputs,evalTime);
        std::vector<double> output = _nn->outf();

        if(output_noise_lvl > 0.0){
            for(double &o : output){
                o = randomNum->normalDist(o,output_noise_lvl);;
            }
        }

        return output;
    }


    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    void init_nn(int nb_input, int nb_hidden, int nb_output,const std::vector<double> &weights, const std::vector<double> &biases, const std::vector<int> &joint_subs){
        _nn = std::make_shared<ff_cpg_t>(nb_input,nb_hidden,nb_output, joint_subs);
        if(_nn->get_nb_connections() != weights.size() || _nn->get_nb_neurons() != biases.size()){
            std::cerr << "NN2 init error: Wrong number of parameters" << std::endl;
            std::cerr << "nb inputs: " << nb_input << " - nb outputs: " << nb_output << " - nb hidden: " << nb_hidden << std::endl;
            std::cerr << "weights: " << weights.size() << " - nb of connections: " << _nn->get_nb_connections() << " - biases: " << biases.size() << " - nb of neurons: " << _nn->get_nb_neurons() << std::endl;
            std::cerr << "joint substrate: ";
            for(int i: joint_subs)
                std::cerr << i << ";";
            std::cerr << std::endl;
            return;
        }
        _nn->set_all_weights(weights);
        _nn->set_all_biases(biases);
        _nn->init();
    }

    static void nbr_parameters_cpg(int nb_input,int nb_hidden,int nb_output, int &nbr_weights, int &nbr_biases, const std::vector<int> &joint_subs){
        ff_cpg_t nn(nb_input,nb_hidden,nb_output,joint_subs);
        nbr_weights = nn.get_nb_connections();
        nbr_biases = nn.get_nb_neurons();
    }
    std::shared_ptr<ff_cpg_t> _nn;

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
