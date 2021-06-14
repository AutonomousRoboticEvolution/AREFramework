#include "CMAESIndividual.hpp"

using namespace are;

void CMAESIndividual::createController(){
    control.reset(new NNControl);
    control->set_parameters(parameters);
    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

    NNGenome nn_constructor;
    nn_constructor.set_parameters(parameters);
    nn_constructor.init();
    NEAT::NeuralNetwork &nn = std::dynamic_pointer_cast<NNControl>(control)->nn;
    nn_constructor.buildPhenotype(nn);


    for(int i = 0; i < weights.size(); i++)
        nn.m_connections[i].m_weight = weights[i];
    for(int i = 0; i < bias.size(); i++)
        nn.m_neurons[i].m_bias = bias[i];


}

void CMAESIndividual::createMorphology(){
    std::string robot = settings::getParameter<settings::String>(parameters,"#robot").value;

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
