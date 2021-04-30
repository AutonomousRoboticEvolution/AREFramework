#include "ARE/nn2/NN2CPPNGenome.hpp"

using namespace are;

std::mt19937 nn2::rgen_t::gen;

float cppn_params::_rate_add_neuron = 0.1;
float cppn_params::_rate_del_neuron = 0.01;
float cppn_params::_rate_add_conn = 0.1;
float cppn_params::_rate_del_conn = 0.01;
float cppn_params::_rate_change_conn = 0.1;

size_t cppn_params::_min_nb_neurons = 2;
size_t cppn_params::_max_nb_neurons = 30;
size_t cppn_params::_min_nb_conns = 1;
size_t cppn_params::_max_nb_conns = 100;

int cppn_params::nb_inputs = 4;
int cppn_params::nb_outputs = 6;
