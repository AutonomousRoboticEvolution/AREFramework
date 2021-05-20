#include "visualization.hpp"


using namespace are;
namespace  fs = boost::filesystem;
namespace st = settings;

void VisuInd::createMorphology(){
    bool is_neat_genome = settings::getParameter<settings::Boolean>(parameters,"#isNEATGenome").value;


    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology.reset(new CPPNMorph(parameters));
    morphology->set_randNum(randNum);

    if(is_neat_genome){
        NEAT::Genome gen =
                std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
        NEAT::NeuralNetwork nn;
        gen.BuildPhenotype(nn);
        std::dynamic_pointer_cast<CPPNMorph>(morphology)->setNEATCPPN(nn);
    }else{
        nn2_cppn_t gen = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
        std::dynamic_pointer_cast<CPPNMorph>(morphology)->setNN2CPPN(gen);
    }
    std::dynamic_pointer_cast<CPPNMorph>(morphology)->createAtPosition(init_x,init_y,init_z);


//    float pos[3];
//    simGetObjectPosition(std::dynamic_pointer_cast<CPPNMorph>(morphology)->getMainHandle(),-1,pos);
}

void VisuInd::createController(){

    bool empty_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    if(empty_gen)
        return;

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;

    int wheel_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_wheelNumber();
    int joint_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_jointNumber();
    int sensor_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_sensorNumber();

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

    bool empty_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    if(empty_gen)
        return;

    std::vector<double> inputs = morphology->update();

    std::vector<double> outputs = control->update(inputs);

    std::dynamic_pointer_cast<CPPNMorph>(morphology)->command(outputs);
}

void Visu::init(){


    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    int generation = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;
    int indIdx = settings::getParameter<settings::Integer>(parameters,"#indToLoad").value;
    bool empty_ctrl_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    bool load_all_morph_gen = settings::getParameter<settings::Boolean>(parameters,"#loadAllMorphGenome").value;
    bool is_neat_genome = settings::getParameter<settings::Boolean>(parameters,"#isNEATGenome").value;

    std::vector<std::string> ctrl_gen_files;
    if(!empty_ctrl_gen){
        if(indIdx < 0){
            int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
            for(int i = 0; i < pop_size; i++){
                std::stringstream sstr2;
                sstr2 << folder_to_load << "/ctrlGenome_" << generation << "_" << i;
                ctrl_gen_files.push_back(sstr2.str());
            }
        }
        else{
            std::stringstream sstr;
            sstr << folder_to_load << "/ctrlGenome_" << generation << "_" << indIdx;
            ctrl_gen_files.push_back(sstr.str());
        }
    }

    std::vector<std::string> morph_gen_files;
    if(load_all_morph_gen){
        std::string filename;
        std::vector<std::string> split_str;
        for(const auto &dirit : fs::directory_iterator(fs::path(folder_to_load))){
            filename = dirit.path().string();
            boost::split(split_str,filename,boost::is_any_of("/"));
            boost::split(split_str,split_str.back(),boost::is_any_of("G"));
            if(split_str[0] == "morph")
                morph_gen_files.push_back(filename);
        }
    }else{
        if(indIdx < 0){
            int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
            for(int i = 0; i < pop_size; i++){
                std::stringstream sstr2;
                sstr2 << folder_to_load << "/morphGenome_" << generation << "_" << i;
                morph_gen_files.push_back(sstr2.str());
            }
        }else{
            std::stringstream sstr2;
            sstr2 << folder_to_load << "/morphGenome_" << generation << "_" << indIdx;
            morph_gen_files.push_back(sstr2.str());
        }
    }


    Genome::Ptr morph_gen;
    Genome::Ptr ctrl_gen;
    //load morphology genome
    for(size_t i = 0; i < morph_gen_files.size(); i++){
        if(is_neat_genome){
            NEAT::Genome neat_gen(morph_gen_files[i].c_str());
            morph_gen.reset(new CPPNGenome(neat_gen));
        }
        else{
            nn2_cppn_t cppn;
            std::ifstream ifs(morph_gen_files[i]);
            boost::archive::text_iarchive iarch(ifs);
            iarch >> cppn;
            morph_gen.reset(new NN2CPPNGenome(cppn));
        }
        morph_gen->set_randNum(randomNum);
        morph_gen->set_parameters(parameters);

        if(empty_ctrl_gen)
            ctrl_gen.reset(new EmptyGenome);
        else{
            ctrl_gen.reset(new NNParamGenome(randomNum,parameters));
            std::dynamic_pointer_cast<NNParamGenome>(ctrl_gen)->from_file(ctrl_gen_files[i]);
        }

        Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }

    morph_gen.reset();
    ctrl_gen.reset();
}

