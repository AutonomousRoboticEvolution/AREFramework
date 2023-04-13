#include "random_controller.hpp"

using namespace are;
namespace fs = boost::filesystem;

bool RandomController::update(const Environment::Ptr &env){
    trajectory = env->get_trajectory();
}

const Genome::Ptr RandomController::get_next_controller_genome(int id){
    std::string experiment_folder = settings::getParameter<settings::String>(parameters,"#repository").value + "/" +
            settings::getParameter<settings::String>(parameters,"#experimentName").value;
    std::string waiting_to_be_evaluated_folder = experiment_folder + "/waiting_to_be_evaluated/";
    std::stringstream genome_file;
    genome_file << waiting_to_be_evaluated_folder << "ctrl_genome_" << id;
    NNParamGenome::Ptr gen(new NNParamGenome);
    if(fs::exists(genome_file.str())){
        ioh::load_controller_genome(experiment_folder,id,gen);

    }else{ //generate a random controller
        std::cout<<"Robot of id " << id <<  " does not have an associated controller genome, so a random one is being created"<<std::endl;

        int wheel=0, joint=0, sensor=0;
        ioh::load_nbr_organs(experiment_folder,id,wheel,joint,sensor);
        make_random_ctrl(wheel,joint,sensor,gen);

    }
    //Filtering the nn parameters
    std::vector<double> weights, biases;
    for(double w: gen->get_weights())
        weights.push_back(std::tanh(w));
    for(double b: gen->get_biases())
        biases.push_back(std::tanh(b));
    gen->set_weights(weights);
    gen->set_biases(biases);
    return gen;
}

void RandomController::load_data_for_update(){


}

void RandomController::write_data_for_update(){
    std::stringstream filepath;
    filepath << "/traj_" << currentIndIndex;

    std::ofstream logFileStream(Logging::log_folder + std::string("/")  + filepath.str(), std::ios::out | std::ios::ate | std::ios::app);

    if(!logFileStream)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/")  + filepath.str() << std::endl;
        return;
    }

    for(const are::waypoint& wp: trajectory)
        logFileStream << wp.to_string() << std::endl;
    logFileStream.close();

}

void RandomController::make_random_ctrl(int wheels, int joints, int sensors, const NNParamGenome::Ptr &ctrl_gen){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;

    int nbr_weights=0, nbr_bias=0, nbr_inputs=0, nbr_outputs=0;
    nbr_inputs = sensors*2;
    nbr_outputs = wheels + joints;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<phy::ffnn_t>::nbr_parameters(nbr_inputs,nb_hidden,nbr_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<phy::rnn_t>::nbr_parameters(nbr_inputs,nb_hidden,nbr_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<phy::elman_t>::nbr_parameters(nbr_inputs,nb_hidden,nbr_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::FCP)
        NN2Control<phy::fcp_t>::nbr_parameters(nbr_inputs,nb_hidden,nbr_outputs,nbr_weights,nbr_bias);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);
    weights = randomNum->randVectd(-max_weight,max_weight,nbr_weights);
    biases  = randomNum->randVectd(-max_weight,max_weight,nbr_bias);

    ctrl_gen->set_weights(weights);
    ctrl_gen->set_biases(biases);
    ctrl_gen->set_nn_type(nn_type);
    ctrl_gen->set_nbr_input(nbr_inputs);
    ctrl_gen->set_nbr_hidden(nb_hidden);
    ctrl_gen->set_nbr_output(nbr_outputs);
}


