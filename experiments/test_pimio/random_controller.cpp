#include "random_controller.hpp"

using namespace are;
namespace fs = boost::filesystem;

void RandomController::init_pop(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<pi::ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<pi::rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<pi::elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);
    weights = randomNum->randVectd(-max_weight,max_weight,nbr_weights);
    biases  = randomNum->randVectd(-max_weight,max_weight,nbr_bias);

    for(int u = 0; u < pop_size; u++){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        ctrl_gen->set_weights(weights);
        ctrl_gen->set_biases(biases);
        Individual::Ptr ind(new pi::NN2Individual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void RandomController::load_data_for_update(){
    std::string ctrl_gen_folder = settings::getParameter<settings::String>(parameters,"#ctrlGenomeFolder").value;

    std::string filename;
    std::vector<std::string> split_str;
    for(const auto &dirit : fs::directory_iterator(fs::path(ctrl_gen_folder))){
        filename = dirit.path().string();
        boost::split(split_str,filename,boost::is_any_of("/"));
        boost::split(split_str,split_str.back(),boost::is_any_of("_"));
        if(split_str[0] == "genome"){
            EmptyGenome::Ptr morph_gen(new EmptyGenome);
            NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
            ctrl_gen->from_file(filename);
            Individual::Ptr ind(new pi::NN2Individual(morph_gen,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }
    }
}

void RandomController::init(){
//    init_pop();
}

void RandomController::init_next_pop(){
    init_pop();
}
