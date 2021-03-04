#include "robustness_test.hpp"


using namespace are;
namespace  fs = boost::filesystem;
namespace st = settings;

void RobustInd::createMorphology(){
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

void RobustInd::createController(){
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

void RobustInd::update(double delta_time){
    std::vector<double> inputs = morphology->update();

    std::vector<double> outputs = control->update(inputs);

    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->command(outputs);
}


std::string RobustInd::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<RobustInd>();
    oarch.register_type<CPPNGenome>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void RobustInd::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<RobustInd>();
    iarch.register_type<CPPNGenome>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}


void RobustnessTest::init(){
    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    int gen_to_load = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;

    list_files_pair_t list_gen_files;
    load_gen_files(list_gen_files,folder_to_load,gen_to_load);

    population.resize(list_gen_files.size());
    for(size_t i = 0; i < list_gen_files.size(); i++){
        NEAT::Genome neat_genome(list_gen_files[i].first.c_str());
        CPPNGenome::Ptr morph_gen(new CPPNGenome(neat_genome));
        morph_gen->set_randNum(randomNum);
        morph_gen->set_parameters(parameters);
        NNParamGenome::Ptr genome(new NNParamGenome(randomNum,parameters));
        genome->from_file(list_gen_files[i].second);

        Individual::Ptr ind(new RobustInd(morph_gen,genome));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population[i] = ind;
    }
}

void RobustnessTest::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool RobustnessTest::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<RobustInd>(ind)->set_final_position(env->get_final_position());
        std::dynamic_pointer_cast<RobustInd>(ind)->set_trajectory(env->get_trajectory());
    }

    return true;
}

void RobustnessTest::load_gen_files(list_files_pair_t &list_gen_files, const std::string &folder, int gen_to_load){
    std::map<int,std::string> morph_files, ctrl_files;
    std::string filename;
    std::vector<std::string> split_str;
    for(const auto &dirit : fs::directory_iterator(fs::path(folder))){
        filename = dirit.path().string();
        boost::split(split_str,filename,boost::is_any_of("/"));
        boost::split(split_str,split_str.back(),boost::is_any_of("_"));

        if(split_str[0] == "morphGenome" && std::stoi(split_str[1]) == gen_to_load)
            morph_files.emplace(stoi(split_str[1])*100+stoi(split_str[2]),filename);
        else if(split_str[0] == "ctrlGenome" && std::stoi(split_str[1]) == gen_to_load)
            ctrl_files.emplace(stoi(split_str[1])*100+stoi(split_str[2]),filename);

    }
    for(const auto& elt: morph_files){
        std::pair<std::string,std::string> file_pair;
        file_pair.first = elt.second;
        file_pair.second = ctrl_files[elt.first];
        list_gen_files.push_back(file_pair);
    }
}
