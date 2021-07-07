#include "ARE/nn2/NN2CPPNGenome.hpp"

using namespace are;

std::mt19937 nn2::rgen_t::gen;

int static_id = 0;

float cppn_params::_rate_add_neuron = 0.5;
float cppn_params::_rate_del_neuron = 0.05;
float cppn_params::_rate_add_conn = 0.2;
float cppn_params::_rate_del_conn = 0.02;
float cppn_params::_rate_change_conn = 0.1;

size_t cppn_params::_min_nb_neurons = 10;
size_t cppn_params::_max_nb_neurons = 50;
size_t cppn_params::_min_nb_conns = 100;
size_t cppn_params::_max_nb_conns = 10000;

int cppn_params::nb_inputs = 4;
int cppn_params::nb_outputs = 6;
