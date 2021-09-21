#include "ARE/nn2/NN2CPPNGenome.hpp"

using namespace are;

std::mt19937 nn2::rgen_t::gen;

int static_id = 0;

float cppn_params::cppn::_rate_add_neuron = 0.2f;
float cppn_params::cppn::_rate_del_neuron = 0.05f;
float cppn_params::cppn::_rate_add_conn = 0.5f;
float cppn_params::cppn::_rate_del_conn = 0.02f;
float cppn_params::cppn::_rate_change_conn = 0.1f;

size_t cppn_params::cppn::_min_nb_neurons = 20;
size_t cppn_params::cppn::_max_nb_neurons = 100;
size_t cppn_params::cppn::_min_nb_conns = 100;
size_t cppn_params::cppn::_max_nb_conns = 50000;

int cppn_params::cppn::nb_inputs = 4;
int cppn_params::cppn::nb_outputs = 6;


float cppn_params::evo_float::mutation_rate = 0.1f;
float cppn_params::evo_float::cross_rate = 0.f;
nn2::evo_float::mutation_t cppn_params::evo_float::mutation_type = nn2::evo_float::polynomial;
nn2::evo_float::cross_over_t cppn_params::evo_float::cross_over_type = nn2::evo_float::no_cross_over;
float cppn_params::evo_float::eta_m = 15.f;
float cppn_params::evo_float::eta_c = 15.f;

void NN2CPPNGenomeLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();

    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << std::dynamic_pointer_cast<NN2CPPNGenome>(
                        ea->get_population()[i]->get_morph_genome()
                        )->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<NN2CPPNGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->to_string();
        logFileStream.close();
    }
}

void NbrConnNeurLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->id() << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()
                      << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()
                      << std::endl;
    }
    logFileStream.close();
}

void ParentingLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->id() << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<NN2CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}

