#include "reevaluateFixed.hpp"


using namespace are;
namespace  fs = boost::filesystem;


void ReevaluateFixed::init(){


    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    int generation = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;
    int ind = settings::getParameter<settings::Integer>(parameters,"#indToLoad").value;



    std::string folder = repository + "/" + folder_to_load;

    std::vector<std::string> gen_files;
    std::vector<int> gen_index;
    if(ind >= 0){
        std::stringstream sstr;
        sstr << folder << "/genome_" << generation << "_" << ind;
        gen_files.push_back(sstr.str());
    }
    else{
        std::string filename;
        std::vector<std::string> split_str;
        for(const auto &dirit : fs::directory_iterator(fs::path(folder))){
            filename = dirit.path().string();
            boost::split(split_str,filename,boost::is_any_of("/"));
            boost::split(split_str,split_str.back(),boost::is_any_of("_"));
            if(split_str[0] == "genome" &&
                    std::stoi(split_str[1]) == generation){

                gen_files.push_back(filename);
                gen_index.push_back(std::stoi(split_str[2]));
            }
        }
    }

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

    population.resize(gen_files.size());
    for(size_t i = 0; i < gen_files.size(); i++){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr genome;

        genome.reset(new NNParamGenome(randomNum,parameters));
        NNParamGenome::Ptr nngenome = std::dynamic_pointer_cast<NNParamGenome>(genome);
        std::ifstream logFileStream;
        logFileStream.open(gen_files[i]);
        std::string line;
        std::getline(logFileStream,line);
        int nbr_weights = std::stoi(line);
        std::getline(logFileStream,line);
        int nbr_bias = std::stoi(line);

        std::vector<double> weights;
        for(int i = 0; i < nbr_weights; i++){
            std::getline(logFileStream,line);
            weights.push_back(std::stod(line));
        }
        nngenome->set_weights(weights);

        std::vector<double> biases;
        for(int i = 0; i < nbr_bias; i++){
            std::getline(logFileStream,line);
            biases.push_back(std::stod(line));
        }
        nngenome->set_biases(biases);


        Individual::Ptr ind(new NN2Individual(morph_gen,genome));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population[gen_index[i]] = ind;
    }
}

void ReevaluateFixed::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool ReevaluateFixed::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_trajectory(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_trajectory());
    }

    return true;
}


