#include "physicalER/sim_update/sim_are_control.hpp"
#include <cmath>

using namespace are::sim;
namespace st = are::settings;


void AREIndividual::createMorphology(){
    morphology = std::make_shared<sim::Morphology_CPPNMatrix>(parameters);
    morphology->set_randNum(randNum);
    std::vector<double> init_pos = st::getParameter<st::Sequence<double>>(parameters,"#initPosition").value;
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);
}

void AREIndividual::createController(){
    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();
    nn_type = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nn_type();
    nb_input = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_input();
    nb_hidden = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_hidden();
    nb_output = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_output();

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
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

}


void AREIndividual::update(double delta_time){
   std::vector<double> inputs = morphology->update();
   std::vector<double> outputs = control->update(inputs);
   morphology->command(outputs);

}


std::string AREIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<AREIndividual>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void AREIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<AREIndividual>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}


AREControl::AREControl(const AREIndividual &ind , std::string stringListOfOrgans, settings::ParametersMapPtr parameters){
    controller = ind;
    _max_eval_time = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value; // in milliseconds
    _time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value; // in milliseconds
    std::cout<<"Target timestep: "<<_time_step<<" ms"<<std::endl;

    // initilise the camera
    // If this is true, the camera input (binary on/off) will be used the first input to the neural network controller:
    cameraInputToNN =  settings::getParameter<settings::Boolean>(parameters,"#useArucoAsInput").value;
    _is_ready = true;
    controller.init();
}


int AREControl::exec(zmq::socket_t& socket, float sim_time){

    if(sim_time <= _max_eval_time){
        // start a new line of log file, and add current time
        AREControl::_sent_finish_mess = false;

        controller.update(sim_time);
    
        // send the new values to the actuators
        
        // the are-update running on the PC expects to get a message on every timestep:
        are::phy::send_string_no_reply("busy",socket,"pi ");

        return 1;
    }else{
        
    
        // send finished message
        if(!_sent_finish_mess){
            are::phy::send_string_no_reply("finish",socket,"pi ");
            _sent_finish_mess = true;
        }
    
        // send log data
        are::phy::send_string_no_reply( logs_to_send.str() ,socket, "pi ");
        are::phy::send_string_no_reply("finished_logs",socket, "pi ");
    
        return 0;
    }
}
