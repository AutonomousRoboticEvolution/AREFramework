#include "ARE/nn2/NN2CPPNGenome.hpp"

using namespace are;
using namespace nn2_cppn;

std::mt19937 nn2::rgen_t::gen;

int cppn_params::cppn::_mutation_type = 0; //uniform
bool cppn_params::cppn::_mutate_connections = true;
bool cppn_params::cppn::_mutate_neurons = true;
float cppn_params::cppn::_mutation_rate = 0.5f;
float cppn_params::cppn::_rate_mutate_conn = 0.1f;
float cppn_params::cppn::_rate_mutate_neur = 0.1f;
float cppn_params::cppn::_rate_add_neuron = 0.1f;
float cppn_params::cppn::_rate_del_neuron = 0.1f;
float cppn_params::cppn::_rate_add_conn = 0.1f;
float cppn_params::cppn::_rate_del_conn = 0.1f;
float cppn_params::cppn::_rate_change_conn = 0.1f;
float cppn_params::cppn::_rate_crossover = 0.5f;

size_t cppn_params::cppn::_min_nb_neurons = 0;
size_t cppn_params::cppn::_max_nb_neurons = 5;
size_t cppn_params::cppn::_min_nb_conns = 10;
size_t cppn_params::cppn::_max_nb_conns = 100;

int cppn_params::cppn::nb_inputs = 4;
int cppn_params::cppn::nb_outputs = 6;

float cppn_params::evo_float::mutation_rate = 0.1f;



void NN2CPPNGenomeLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<NN2CPPNGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->to_string();
        logFileStream.close();
    }
}

void NbrConnNeurLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()
                      << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()
                      << std::endl;
    }
    logFileStream.close();
}

void ParentingLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}

void GraphVizLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "cppn_" << std::dynamic_pointer_cast<NN2CPPNGenome>(
                        ea->get_population()[i]->get_morph_genome()
                        )->id() << ".dot";
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->get_cppn();
        cppn.write_dot(logFileStream);
        logFileStream.close();
    }
}
