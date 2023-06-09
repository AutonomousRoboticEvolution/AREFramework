#include "simulatedER/nn2/NN2Individual.hpp"

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
        control.reset(new NN2Control<ffnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else if(nn_type == st::nnType::ELMAN){
        control.reset(new NN2Control<elman_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);

    }
    else if(nn_type == st::nnType::RNN){
        control.reset(new NN2Control<rnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else if(nn_type == st::nnType::FCP){

        control.reset(new NN2Control<fcp_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<fcp_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<fcp_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias);
    }
    else if(nn_type == st::nnType::ELMAN_CPG){
        control.reset(new NN2Control<elman_cpg_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_cpg_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_cpg_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias, joint_subs);
    }
    else if(nn_type == st::nnType::CPG){
        control.reset(new NN2Control<cpg_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<cpg_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<cpg_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias, joint_subs);
    }
    else if(nn_type == st::nnType::FF_CPG){
        control.reset(new NN2Control<ff_cpg_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ff_cpg_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ff_cpg_t>>(control)->init_nn(nb_input,nb_hidden,nb_output,weights,bias, joint_subs);
    }
    else {
        std::cerr << "ERROR: unknown type of neural network" << std::endl;
        return;
    }

}

void NN2Individual::createMorphology(){
    morphology.reset(new sim::FixedMorphology(parameters));
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    morphology->set_randNum(randNum);


    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;

    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);
}

void NN2Individual::update(double delta_time){
    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;
    double diff = delta_time/ctrl_freq - std::trunc(delta_time/ctrl_freq);
    if( diff < 0.1){
        std::vector<double> inputs = morphology->update();
        std::vector<double> outputs = control->update(inputs);
        morphology->command(outputs);
    }

    energy_cost+=std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_energy_cost();

    sim_time = delta_time;
}

void NN2Individual::crossover(const Individual::Ptr &partner, Individual* child){
    Genome::Ptr child_genome(new NNParamGenome);
    EmptyGenome::Ptr empty_gen(new EmptyGenome);
    std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->crossover(partner->get_ctrl_genome(),child_genome);
    child_genome->set_parameters(parameters);
    child_genome->set_randNum(randNum);
    *static_cast<NN2Individual*>(child) = NN2Individual(empty_gen,std::dynamic_pointer_cast<NNParamGenome>(child_genome));
    child->set_randNum(randNum);
    child->set_parameters(parameters);

}

void NN2Individual::symmetrical_crossover(const Individual::Ptr &partner, Individual* child1, Individual* child2){
    Genome::Ptr child_genome1(new NNParamGenome);
    Genome::Ptr child_genome2(new NNParamGenome);
    EmptyGenome::Ptr empty_gen(new EmptyGenome);
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
