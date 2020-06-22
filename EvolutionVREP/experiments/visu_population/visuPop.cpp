#include "visuPop.hpp"


using namespace are;
namespace  fs = boost::filesystem;

//void VisuIndividual::createController(){
//    int genType = settings::getParameter<settings::Integer>(parameters,"#genType").value;
//    control.reset(new NNControl);
//    control->set_parameters(parameters);
//    std::dynamic_pointer_cast<NNControl>(control)->set_randonNum(randNum);

//    if(genType == settings::NEAT){
////todo
//    }else if(genType == settings::NN){
//        NEAT::NeuralNetwork &nn = std::dynamic_pointer_cast<NNControl>(control)->nn;
//        std::dynamic_pointer_cast<NNGenome>(ctrlGenome)->buildPhenotype(nn);
//    }else if(genType == settings::NNPARAM){
//        std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
//        std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

//        NNGenome nn_constructor;
//        nn_constructor.set_parameters(parameters);
//        nn_constructor.init();
//        NEAT::NeuralNetwork &nn = std::dynamic_pointer_cast<NNControl>(control)->nn;
//        nn_constructor.buildPhenotype(nn);

//        for(int i = 0; i < weights.size(); i++)
//            nn.m_connections[i].m_weight = weights[i];
//        for(int i = 0; i < bias.size(); i++)
//            nn.m_neurons[i].m_bias = bias[i];
//    }

//}

//void VisuIndividual::createMorphology(){
//    std::string robot = settings::getParameter<settings::String>(parameters,"#robot").value;


//    morphology.reset(new FixedMorphology(parameters));
//    std::dynamic_pointer_cast<FixedMorphology>(morphology)->loadModel();
//    morphology->set_randNum(randNum);

//    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
//    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
//    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

//    morphology->createAtPosition(init_x,init_y,init_z);
//}

//void VisuIndividual::update(double delta_time){
//    std::string robot = settings::getParameter<settings::String>(parameters,"#robot").value;

//    std::vector<double> inputs = morphology->update();

//    std::vector<double> outputs = control->update(inputs);
//    std::vector<int> jointHandles;
//    jointHandles =
//               std::dynamic_pointer_cast<FixedMorphology>(morphology)->get_jointHandles();

//    assert(jointHandles.size() == outputs.size());

//    for (size_t i = 0; i < outputs.size(); i++){
//        simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));
//    }
//}

void VisuPop::init(){


    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    int generation = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;
    int ind = settings::getParameter<settings::Integer>(parameters,"#indToLoad").value;



    std::string folder = repository + "/" + folder_to_load;

    std::vector<std::string> gen_files;
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

    for(const std::string& file : gen_files){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr genome;

        genome.reset(new NNParamGenome(randomNum,parameters));
        NNParamGenome::Ptr nngenome = std::dynamic_pointer_cast<NNParamGenome>(genome);
        std::ifstream logFileStream;
        logFileStream.open(file);
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
        population.push_back(ind);
    }
}

//void VisuPop::init(){

//    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
//    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
//    int generation = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;
//    int ind = settings::getParameter<settings::Integer>(parameters,"#indToLoad").value;
//    int genType = settings::getParameter<settings::Integer>(parameters,"#genType").value;

//    std::string folder = repository + "/" + folder_to_load;

//    std::vector<std::string> gen_files;
//    if(ind >= 0){
//        std::stringstream sstr;
//        sstr << folder << "/genome_" << generation << "_" << ind;
//        gen_files.push_back(sstr.str());
//    }
//    else{
//        std::string filename;
//        std::vector<std::string> split_str;
//        for(const auto &dirit : fs::directory_iterator(fs::path(folder))){
//            filename = dirit.path().string();
//            boost::split(split_str,filename,boost::is_any_of("/"));
//            boost::split(split_str,split_str.back(),boost::is_any_of("_"));
//            if(split_str[0] == "genome" &&
//                    std::stoi(split_str[1]) == generation){
//                gen_files.push_back(filename);
//            }
//        }
//    }

//    for(const std::string& file: gen_files){
//        EmptyGenome::Ptr morph_gen(new EmptyGenome);
//        Genome::Ptr genome;

//        if(genType == settings::NEAT){
//            //todo
//        }
//        else if(genType == settings::NN){
//            genome.reset(new NNGenome(randomNum,parameters));
//            NEAT::Genome neat_genome(file.c_str());
//            std::dynamic_pointer_cast<NNGenome>(genome)->set_nn_genome(neat_genome);
//        }else if(genType == settings::NNPARAM){
//            genome.reset(new NNParamGenome(randomNum,parameters));
//            NNParamGenome::Ptr nngenome = std::dynamic_pointer_cast<NNParamGenome>(genome);
//            std::ifstream logFileStream;
//            logFileStream.open(file);
//            std::string line;
//            std::getline(logFileStream,line);
//            int nbr_weights = std::stoi(line);
//            std::getline(logFileStream,line);
//            int nbr_bias = std::stoi(line);

//            std::vector<double> weights;
//            for(int i = 0; i < nbr_weights; i++){
//                std::getline(logFileStream,line);
//                weights.push_back(std::stod(line));
//            }
//            nngenome->set_weights(weights);


//            std::vector<double> biases;
//            for(int i = 0; i < nbr_bias; i++){
//                std::getline(logFileStream,line);
//                biases.push_back(std::stod(line));
//            }
//            nngenome->set_biases(biases);


//        }


//        VisuIndividual::Ptr ind(new VisuIndividual(morph_gen,genome));
//        ind->set_parameters(parameters);
//        ind->set_randNum(randomNum);
//        population.push_back(ind);
//    }

//}
