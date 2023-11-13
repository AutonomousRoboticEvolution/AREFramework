#include "NN2Individual.hpp"

using namespace are::sim;
namespace st = are::settings;

void NN2Individual::createController(){

    int nn_type = st::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_input = st::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    int nb_hidden = st::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    int nb_output = st::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;
    std::vector<int> joint_subs = st::getParameter<settings::Sequence<int>>(parameters,"#jointSubs").value;

    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();


    if(nn_type == st::nnType::FFNN){
        control = std::make_shared<NN2Control<ffnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else if(nn_type == st::nnType::ELMAN){
        control = std::make_shared<NN2Control<elman_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);

    }
    else if(nn_type == st::nnType::RNN){
        control = std::make_shared<NN2Control<rnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else if(nn_type == st::nnType::FCP){
        control = std::make_shared<NN2Control<fcp_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<fcp_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<fcp_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else if(nn_type == st::nnType::ELMAN_CPG){
        control = std::make_shared<NN2Control<elman_cpg_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_cpg_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_cpg_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias, joint_subs);
    }
    else {
        std::cerr << "ERROR: unknown type of neural network" << std::endl;
        return;
    }

}

void NN2Individual::createMorphology(){
    morphology = std::make_shared<sim::FixedMorphology>(parameters);
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    morphology->set_randNum(randNum);

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->createAtPosition(init_x,init_y,init_z);
}

void NN2Individual::update(double delta_time){
    std::vector<double> inputs = morphology->update();

    std::vector<double> outputs = control->update(inputs);

    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->command(outputs);
    energy_cost+=std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_energy_cost();
    sim_time = delta_time;
}

void NN2Individual::crossover(const Individual::Ptr &partner, Individual* child){
    Genome::Ptr child_genome = std::make_shared<NNParamGenome>();
    EmptyGenome::Ptr empty_gen = std::make_shared<EmptyGenome>();
    std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->crossover(partner->get_ctrl_genome(),child_genome);
    child_genome->set_parameters(parameters);
    child_genome->set_randNum(randNum);
	*static_cast<NN2Individual*>(child) = NN2Individual(empty_gen,std::dynamic_pointer_cast<NNParamGenome>(child_genome));
    child->set_randNum(randNum);
    child->set_parameters(parameters);
}

void NN2Individual::symmetrical_crossover(const Individual::Ptr &partner, Individual* child1, Individual* child2){
    Genome::Ptr child_genome1 = std::make_shared<NNParamGenome>();
    Genome::Ptr child_genome2 = std::make_shared<NNParamGenome>();
    EmptyGenome::Ptr empty_gen = std::make_shared<EmptyGenome>();
    std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->symmetrical_crossover(partner->get_ctrl_genome(),child_genome1,child_genome2);
    child_genome1->set_parameters(parameters);
    child_genome2->set_parameters(parameters);
    child_genome1->set_randNum(randNum);
    child_genome2->set_randNum(randNum);
	*static_cast<NN2Individual*>(child1) = NN2Individual(empty_gen,std::dynamic_pointer_cast<NNParamGenome>(child_genome1));
	*static_cast<NN2Individual*>(child2) = NN2Individual(empty_gen,std::dynamic_pointer_cast<NNParamGenome>(child_genome2));
    child1->set_randNum(randNum);
    child2->set_randNum(randNum);
    child1->set_parameters(parameters);
    child2->set_parameters(parameters);
}

std::string NN2Individual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<NN2Individual>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void NN2Individual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<NN2Individual>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}
void NN2Individual::compute_fitness(){
    double fitness = 0;
    for(const auto &r : rewards)
        fitness += r;
    fitness /= static_cast<double>(rewards.size());
    objectives[0] = fitness;
//    copy_rewards = rewards;
}
