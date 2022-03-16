#include "CPPNIndividual.h"

using namespace are;

void CPPNIndividual::createMorphology()
{
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
    morphology.reset(new sim::Morphology_CPPNMatrix(parameters));
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNEATCPPN(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,0.15);
    if(std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_morph_desc().defined())
        assert(std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_morph_desc() == std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());

    std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->set_morph_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());
    setGenome();
    setMorphDesc();
    setManRes();
}

void CPPNIndividual::setGenome()
{
    nn = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getNEATCPPN();
}

void CPPNIndividual::setMorphDesc()
{
    morphDesc = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getMorphDesc();
}

void CPPNIndividual::setManRes()
{
    testRes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getRobotManRes();
}

Eigen::VectorXd CPPNIndividual::descriptor(){
    return morphDesc;
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

    //set parameters and randNum to the genome as it is not contained in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}
