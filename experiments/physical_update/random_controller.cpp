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
        NN2Control<phy::ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<phy::rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<phy::elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::FCP)
        NN2Control<phy::fcp_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
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
        ctrl_gen->set_nbr_input(nb_input);
        ctrl_gen->set_nbr_hidden(nb_hidden);
        ctrl_gen->set_nbr_output(nb_output);
        Individual::Ptr ind(new phy::NN2Individual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void RandomController::load_data_for_update(){

    std::string waiting_to_be_evaluated_folder = settings::getParameter<settings::String>(parameters,"#ctrlGenomeFolder").value + "waiting_to_be_evaluated/";

    // loop through all list_of_organs files, and if a coresponding controller genome file does not exist, generate a random one
    std::string filepath, filename;
    std::vector<std::string> split_str;
    for(const auto &dirit : fs::directory_iterator(fs::path(waiting_to_be_evaluated_folder))){
        filepath = dirit.path().string();
        boost::split(split_str,filepath,boost::is_any_of("/"));
        filename = split_str.back();
        boost::split(split_str,filename,boost::is_any_of("_"));
        if(split_str[0]+"_"+split_str[1]+"_"+split_str[2] == "list_of_organs"){
            // get the robot ID
            std::string robotID = filename.substr(15,filename.length()-19);

            // check if a conroller file already exists
            if (fs::exists(waiting_to_be_evaluated_folder+"ctrl_genome_"+robotID)){
                std::cout<<"file ctrl_genome_"<<robotID<<" already exists"<<std::endl;
            } else {
                // doesn't exist, so make a new random controller and save it as a file
                std::cout<<"WARNING the file "<<filename+" does not have an associated controller genome, so a random one is being created"<<std::endl;

                // determine number of inputs and outputs:
                std::string thisLine;
                std::ifstream temp_file_stream(filepath);
                int numberOfInputs=0;
                int numberOfOutputs=0;
                while( std::getline(temp_file_stream, thisLine,'\n') ){
                    std::string organType = thisLine.substr(0, thisLine.find(","));
                    if (organType=="0") {} //Head
                    if (organType=="1") numberOfOutputs++ ; //wheel
                    if (organType=="2") numberOfInputs+=2 ; //sensor
                }
                std::cout<<"numberOfInputs: "<<numberOfInputs<<std::endl;
                std::cout<<"numberOfOutputs: "<<numberOfOutputs<<std::endl;

                NNParamGenome::Ptr ctrl_gen = makeRandomController(numberOfInputs, numberOfOutputs);

                // now write to file
                std::ofstream fileOut(waiting_to_be_evaluated_folder+"ctrl_genome_"+robotID);
                fileOut<<ctrl_gen->to_string();
                std::cout<<"new random controller genome written to "<<waiting_to_be_evaluated_folder+"ctrl_genome_"+robotID<<std::endl;

            }
        }
    }
}

NNParamGenome::Ptr RandomController::makeRandomController(int numberOfInputs, int numberOfOutputs){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<phy::ffnn_t>::nbr_parameters(numberOfInputs,nb_hidden,numberOfOutputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<phy::rnn_t>::nbr_parameters(numberOfInputs,nb_hidden,numberOfOutputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<phy::elman_t>::nbr_parameters(numberOfInputs,nb_hidden,numberOfOutputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::FCP)
        NN2Control<phy::fcp_t>::nbr_parameters(numberOfInputs,nb_hidden,numberOfOutputs,nbr_weights,nbr_bias);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return nullptr;
    }

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);
    weights = randomNum->randVectd(-max_weight,max_weight,nbr_weights);
    biases  = randomNum->randVectd(-max_weight,max_weight,nbr_bias);


    NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
    ctrl_gen->set_weights(weights);
    ctrl_gen->set_biases(biases);
    ctrl_gen->set_nbr_input(numberOfInputs);
    ctrl_gen->set_nbr_hidden(nb_hidden);
    ctrl_gen->set_nbr_output(numberOfOutputs);

    return ctrl_gen;
}

void RandomController::init(){
//    init_pop();
}

void RandomController::init_next_pop(){
    init_pop();
}
