#include "visualization.hpp"


using namespace are;
namespace  fs = boost::filesystem;
namespace st = settings;

void VisuInd::createMorphology(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();

    morphology.reset(new Morphology_CPPNMatrix(parameters));
    morphology->set_randNum(randNum);
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);
    float pos[3];
    simGetObjectPosition(morphology->getMainHandle(),-1,pos);
}

void VisuInd::createController(){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;

    int wheel_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getIndDesc().cartDesc.wheelNumber;
    int joint_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getIndDesc().cartDesc.jointNumber;
    int sensor_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getIndDesc().cartDesc.sensorNumber;

    int nb_inputs = sensor_nbr*2;
    int nb_outputs = wheel_nbr + joint_nbr;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

    std::cout << "number of weights : " << nbr_weights << " and number of biases : " << nbr_bias << std::endl;

    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

    if(nn_type == st::nnType::FFNN){
        control.reset(new NN2Control<ffnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias);
    }
    else if(nn_type == st::nnType::ELMAN){
        control.reset(new NN2Control<elman_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias);

    }
    else if(nn_type == st::nnType::RNN){
        control.reset(new NN2Control<rnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias);
    }
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

}

void VisuInd::update(double delta_time){
    std::vector<double> inputs = morphology->update();

    std::vector<double> outputs = control->update(inputs);

    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->command(outputs);
}

void Visu::init(){


    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;


    std::string ctrl_gen_file;
    std::stringstream sstr;
    sstr << folder_to_load << "/ctrl_genome";
    ctrl_gen_file = sstr.str();

    std::string morph_gen_file;
    std::stringstream sstr2;
    sstr2 << folder_to_load << "/morph_genome";
    morph_gen_file = sstr2.str();


    population.resize(1);
    NNParamGenome::Ptr ctrl_gen(new NNParamGenome);

    //load ctrl genome
    std::ifstream logFileStream;
    logFileStream.open(ctrl_gen_file);
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
    ctrl_gen->set_weights(weights);

    std::vector<double> biases;
    for(int i = 0; i < nbr_bias; i++){
        std::getline(logFileStream,line);
        biases.push_back(std::stod(line));
    }
    ctrl_gen->set_biases(biases);

    ctrl_gen->set_randNum(randomNum);
    ctrl_gen->set_parameters(parameters);

    //load morphology genome
    std::stringstream filepath;
    NEAT::Genome neat_gen(morph_gen_file.c_str());
    CPPNGenome::Ptr morph_gen(new CPPNGenome(neat_gen));
    morph_gen->set_randNum(randomNum);
    morph_gen->set_parameters(parameters);


    Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
    ind->set_parameters(parameters);
    ind->set_randNum(randomNum);
    population[0] = ind;

}

