#include "CPPNIndividual.h"

using namespace are;



void CPPNIndividual::createMorphology()
{
    auto round = [](const Eigen::VectorXd &v) -> Eigen::VectorXd{
        Eigen::VectorXd rv(v.rows());
        for(int i = 0; i < v.rows(); i++)
            rv(i) = std::round(v(i));
        return rv;
    };

    morphology = std::make_shared<sim::Morphology_CPPNMatrix>(parameters);
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],0.15);

    if(ctrlGenome->get_type() != "empty_genome")
       assert(std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cart_desc() == std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_cart_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());

    setMorphDesc();
    setManRes();
    set_organ_position_descriptor();
    set_4d_matrix();
}


void CPPNIndividual::setMorphDesc()
{
    morphDesc = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc();
}

void CPPNIndividual::set_organ_position_descriptor()
{
    organ_position_descriptor = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosDesc();
}

void CPPNIndividual::setManRes()
{
    testRes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getRobotManRes();
}

void CPPNIndividual::set_4d_matrix()
{
    matrix_4d = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->get_matrix_4d();
}

Eigen::VectorXd CPPNIndividual::descriptor(){
    int descriptor = settings::getParameter<settings::Integer>(parameters,"#descriptor").value;
    if(descriptor == CART_DESC)
        return morphDesc.getCartDesc();
    else if(descriptor == ORGAN_POSITION)
        return organ_position_descriptor.getCartDesc();
    else{
        std::cerr << "Unknown descriptor" << std::endl;
        return Eigen::VectorXd::Zero(1);
    }

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
