#include "fixed_controller.hpp"

using namespace are;

std::vector<double> FixedController::update(const std::vector<double> &sensorValues){
    double input_noise_lvl = settings::getParameter<settings::Double>(parameters,"#inputNoiseLevel").value;
    double output_noise_lvl = settings::getParameter<settings::Double>(parameters,"#outputNoiseLevel").value;
    float evalTime = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;

    std::vector<double> inputs = sensorValues;
    if(input_noise_lvl > 0.0){
        for(double &sv : inputs){
            sv = randomNum->normalDist(sv,input_noise_lvl);
        }
    }
    _nn.step(inputs,evalTime);
    std::vector<double> output = _nn.outf();

    if(output_noise_lvl > 0.0){
        for(double &o : output){
            o = randomNum->normalDist(o,output_noise_lvl);;
        }
    }

    return output;
}

void FixedController::init_nn(int nb_input, int nb_hidden, int nb_output){
    _nn = ffnn_t(nb_input,nb_hidden,nb_output);
    int nbr_weights = _nn.get_nb_connections();
    int nbr_biases = _nn.get_nb_neurons();
    std::vector<double> weights = randomNum->randVectd(-0.5,0.5,nbr_weights);
    std::vector<double> biases = randomNum->randVectd(-0.5,0.5,nbr_biases);
    _nn.set_all_weights(weights);
    _nn.set_all_biases(biases);
    _nn.set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
    _nn.init();
}

void FixedController::init_nn(int nb_input, int nb_hidden, int nb_output, double weight, double bias){
    _nn = ffnn_t(nb_input,nb_hidden,nb_output);
    int nbr_weights = _nn.get_nb_connections();
    int nbr_biases = _nn.get_nb_neurons();
    std::vector<double> weights(nbr_weights,weight);
    std::vector<double> biases(nbr_biases,bias);
    _nn.set_all_weights(weights);
    _nn.set_all_biases(biases);
    _nn.set_all_afparams(std::vector<std::vector<double>>(biases.size(),{1,0}));
    _nn.init();
}
