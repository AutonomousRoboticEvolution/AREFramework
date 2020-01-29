#include "CPPNIndividual.h"

using namespace are;

CPPNIndividual::CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
    Individual(morph_gen,ctrl_gen)
{

}

Individual::Ptr CPPNIndividual::clone()
{
    return std::make_shared<CPPNIndividual>(*this);
}

void CPPNIndividual::update(double delta_time)
{
    std::vector<double> inputs = morphology->update();

//    std::cout << "Inputs : ";
//    for(const double& prox : inputs)
//        std::cout << prox << " ; ";
//    std::cout << std::endl;

    std::vector<double> outputs = control->update(inputs);

//    std::cout << "Outputs : ";
//    for(const double& o : outputs)
//        std::cout << o << " ; ";
//    std::cout << std::endl;

    std::vector<int> jointHandles =
            std::dynamic_pointer_cast<EPuckMorphology>(morphology)->get_jointHandles();

    assert(jointHandles.size() == outputs.size());

    for (size_t i = 0; i < outputs.size(); i++){
        simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));
    }
}

void CPPNIndividual::createMorphology()
{
    morphology.reset(new EPuckMorphology(parameters));
    std::dynamic_pointer_cast<EPuckMorphology>(morphology)->loadModel();

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);
}

void CPPNIndividual::createController()
{
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(ctrlGenome)->get_neat_genome();
    NEAT::Substrate subs = morphology->get_substrate();
    control.reset(new NNControl);
//    control->set_properties(properties);
    NEAT::NeuralNetwork nn;
    gen.BuildHyperNEATPhenotype(nn,subs);
    std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
}

std::string CPPNIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<CPPNIndividual>();
    oarch.register_type<CPPNGenome>();
//    oarch.register_type<EmptyGenome>();
    oarch << *this;
    return sstream.str();
}

void CPPNIndividual::from_string(const std::string &str){
    std::cout << "CPPNIndividual" << std::endl;
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CPPNIndividual>();
    iarch.register_type<CPPNGenome>();
//    iarch.register_type<EmptyGenome>();
    iarch >> *this;
}
