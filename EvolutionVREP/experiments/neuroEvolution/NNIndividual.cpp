#include "NNIndividual.hpp"

using namespace are;

void NNIndividual::createController(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<NNGenome>(ctrlGenome)->get_nn_genome();
    control.reset(new NNControl);
    control->set_parameters(parameters);
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
}

void NNIndividual::createMorphology(){
    morphology.reset(new EPuckMorphology(parameters));
    std::dynamic_pointer_cast<EPuckMorphology>(morphology)->loadModel();

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);
}

void NNIndividual::update(double delta_time){
    std::vector<double> inputs = morphology->update();

    std::vector<double> outputs = control->update(inputs);


    std::vector<int> jointHandles =
            std::dynamic_pointer_cast<EPuckMorphology>(morphology)->get_jointHandles();

    assert(jointHandles.size() == outputs.size());

    for (size_t i = 0; i < outputs.size(); i++){
        simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));
    }
}

std::string NNIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<NNIndividual>();
    oarch.register_type<NNGenome>();
    oarch << *this;
    return sstream.str();
}

void NNIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<NNIndividual>();
    iarch.register_type<NNGenome>();
    iarch >> *this;
}
