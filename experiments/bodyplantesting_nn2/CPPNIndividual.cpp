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
//    individual_id = morphGenome->id();

    bool use_quadric = settings::getParameter<settings::Boolean>(parameters,"#useQuadric").value;

    morphology = std::make_shared<sim::Morphology_CPPNMatrix>(parameters);
    if(use_quadric){
        nn2_cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_cppn();
        quadric_t<quadric_params> quadric = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_quadric();
        std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
        std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->set_quadric(quadric);
    }else{
        nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
        std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);

    }
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->set_morph_id(morphGenome->id());
    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],0.15);

    if(!use_quadric){
        if(ctrlGenome->get_type() != "empty_genome")
            assert(std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_feat_desc() == std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getFeatureDesc());
        std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_feature_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getFeatureDesc());
    }

    feature_desc = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getFeatureDesc();
    matrix_descriptor = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getMatrixDesc();
    testRes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getRobotManRes();
    matrix_4d = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->get_matrix_4d();
}




Eigen::VectorXd CPPNIndividual::descriptor(){
    int descriptor = settings::getParameter<settings::Integer>(parameters,"#descriptor").value;
    if(descriptor == sim::FEATURES)
        return feature_desc.to_eigen_vector();
    else if(descriptor == sim::ORGAN_POSITION)
        return matrix_descriptor.to_eigen_vector();
    else{
        std::cerr << "Unknown descriptor" << std::endl;
        return Eigen::VectorXd::Zero(1);
    }

}

std::string CPPNIndividual::to_string() const
{
    bool use_quadric = settings::getParameter<settings::Boolean>(parameters,"#useQuadric").value;


    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<CPPNIndividual>();
    if(use_quadric)
        oarch.register_type<SQCPPNGenome>();
    else
        oarch.register_type<NN2CPPNGenome>();
    oarch << *this;
    return sstream.str();
}

void CPPNIndividual::from_string(const std::string &str){
    bool use_quadric = settings::getParameter<settings::Boolean>(parameters,"#useQuadric").value;

    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CPPNIndividual>();
    if(use_quadric)
        iarch.register_type<SQCPPNGenome>();
    else
        iarch.register_type<NN2CPPNGenome>();
    iarch >> *this;

    //set parameters and randNum to the genome as it is not contained in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}
