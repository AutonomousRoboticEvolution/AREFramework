#include "CPPNIndividual.h"

using namespace are;



void CPPNIndividual::createMorphology()
{
    morphology.reset(new sim::Morphology_CPPNMatrix(parameters));
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,0.15);
    setMorphDesc();
    setManRes();
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
    oarch.register_type<NN2CPPNGenome>();
    oarch << *this;
    return sstream.str();
}

void CPPNIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CPPNIndividual>();
    iarch.register_type<NN2CPPNGenome>();
    iarch >> *this;

    //set parameters and randNum to the genome as it is not contained in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}
