#include "CMAESIndividual.hpp"

using namespace are;
namespace st = settings;

void CMAESIndividual::createController(){

    int nn_type = st::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_input = st::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    int nb_hidden = st::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    int nb_output = st::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;

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
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

}

void CMAESIndividual::createMorphology(){
    std::string robot = st::getParameter<settings::String>(parameters,"#robot").value;

    if(robot == "EPuck"){
        morphology.reset(new EPuckMorphology(parameters));
        std::dynamic_pointer_cast<EPuckMorphology>(morphology)->loadModel();
    }
    else if(robot == "AREPuck"){
        morphology.reset(new AREPuckMorphology(parameters));
        std::dynamic_pointer_cast<AREPuckMorphology>(morphology)->loadModel();
    }

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);
}

void CMAESIndividual::update(double delta_time){
    std::string robot = settings::getParameter<settings::String>(parameters,"#robot").value;

    std::vector<double> inputs = morphology->update();

    std::vector<double> outputs = control->update(inputs);
    std::vector<int> jointHandles;
    if(robot == "EPuck"){
       jointHandles =
               std::dynamic_pointer_cast<EPuckMorphology>(morphology)->get_jointHandles();
    }else if(robot == "AREPuck"){
        jointHandles =
                std::dynamic_pointer_cast<AREPuckMorphology>(morphology)->get_jointHandles();
    }
    assert(jointHandles.size() == outputs.size());

    for (size_t i = 0; i < outputs.size(); i++){
        simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));
    }
}

std::string CMAESIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<CMAESIndividual>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void CMAESIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CMAESIndividual>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;
}
