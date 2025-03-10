#ifndef NN_ANALYSIS_HPP
#define NN_ANALYSIS_HPP
#include "mlp.hpp"
#include "elman.hpp"
#include "rnn.hpp"
#include "fcp.hpp"
#include "elman_cpg.hpp"
#include "cpg.hpp"
#include "ff_cpg.hpp"

namespace tools{
using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using cpg_neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::Af_cpg<>>;
using connection_t = nn2::Connection<double>;
}

using ffnn_t = nn2::Mlp<tools::neuron_t,tools::connection_t>;
using elman_t = nn2::Elman<tools::neuron_t,tools::connection_t>;
using rnn_t = nn2::Rnn<tools::neuron_t,tools::connection_t>;
using fcp_t = nn2::Fcp<tools::neuron_t,tools::connection_t>;
using elman_cpg_t = nn2::ElmanCPG<tools::neuron_t,tools::connection_t>;
using cpg_t = nn2::CPG<tools::cpg_neuron_t,tools::connection_t>;
using ff_cpg_t = nn2::Ffcpg<nn2::ffcpg::neuron_t,nn2::ffcpg::connection_t>;

typedef enum nn_type_t{
    FFNN = 0,
    RNN = 1,
    ELMAN = 2,
    ELMAN_CPG = 3,
    CPG = 4,
    FCP = 5,
    FF_CPG = 6
}nn_type_t;



std::string io_to_string(const std::vector<double> inputs,const std::vector<double> &outputs);

template<typename NN>
void analyse(int nbr_inputs,int nbr_outputs,int nbr_hidden, const std::vector<double> &weights, const std::vector<double> &biases, float step){
    NN nn(nbr_inputs,nbr_hidden,nbr_outputs);
    nn.set_all_weights(weights);
    nn.set_all_biases(biases);
    nn.set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
    nn.init();

    int nbr_steps = static_cast<int>(1/step);
    std::vector<double> inputs(nbr_inputs);
    std::cout << nbr_inputs << ";" << nbr_outputs << std::endl;
    for(int i = -1; i < nbr_inputs; i++){
        for(int j = 0; j < nbr_inputs; j++){
            if(j != i)
                inputs[j] = 0;
        }
        for(int t = -nbr_steps; t <=nbr_steps; t++){
            if(i >= 0)
                inputs[i] = step*static_cast<float>(t);
            nn.step(inputs,step*static_cast<float>(t));
            std::vector<double> outputs = nn.outf();
            std::cout << io_to_string(inputs,outputs) << std::endl;
        }
    }
}

template<typename NN>
void analyse_cpg(int nbr_inputs,int nbr_outputs,int nbr_hidden, const std::vector<int> joint_subs, const std::vector<double> &weights, const std::vector<double> &biases, float step){
    NN nn(nbr_inputs,nbr_hidden,nbr_outputs,joint_subs);
    nn.set_all_weights(weights);
    nn.set_all_biases(biases);
   // nn.set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
    nn.init();

    int nbr_steps = static_cast<int>(1/step);
    std::vector<double> inputs(nbr_inputs);
    std::cout << nbr_inputs << ";" << nbr_outputs << std::endl;
    for(int i = -1; i < nbr_inputs; i++){
        for(int j = 0; j < nbr_inputs; j++){
            if(j != i)
                inputs[j] = 0;
        }
        for(int t = -nbr_steps; t <=nbr_steps; t++){
            if(i >= 0)
                inputs[i] = step*static_cast<float>(t);
            nn.step(inputs,step*static_cast<float>(t));
            std::vector<double> outputs = nn.outf();
            std::vector<double> hidden_out = nn.get_hidden_outf();
            std::vector<double> cpg_out = nn.get_cpg_outf();

            outputs.insert(outputs.end(),hidden_out.begin(),hidden_out.end());
            outputs.insert(outputs.end(),cpg_out.begin(),cpg_out.end());

            std::cout << io_to_string(inputs,outputs) << std::endl;
        }
    }
}



#endif //NN_ANALYSIS_HPP
