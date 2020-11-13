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
    std::vector<double> outputs = control->update(inputs);
    morphology->command(outputs);
}

void CPPNIndividual::createMorphology()
{
    morphology.reset(new sim::FixedMorphology(parameters));
    morphology->set_randNum(randNum);
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->setSubstrate(sim::subtrates::are_puck);

    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,init_z);
}

void CPPNIndividual::createController()
{
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(ctrlGenome)->get_neat_genome();
    NEAT::Substrate subs = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_substrate();
    control.reset(new NNControl);
    control->set_parameters(parameters);
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
    oarch << *this;
    return sstream.str();
}

void CPPNIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CPPNIndividual>();
    iarch.register_type<CPPNGenome>();
    iarch >> *this;
}
