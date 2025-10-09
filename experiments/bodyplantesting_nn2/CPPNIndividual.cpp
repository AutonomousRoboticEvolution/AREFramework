#include "CPPNIndividual.h"
#include "simulatedER/are_morphology.hpp"

using namespace are;

void CPPNIndividual::createMorphology()
{
    int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;

    if(genome_type == morph_genome_type::CPPN){
        morphology = std::make_shared<sim::CPPNMorphology>(parameters);
        nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
        std::dynamic_pointer_cast<sim::CPPNMorphology>(morphology)->set_cppn(cppn);
    }
    else if(genome_type == morph_genome_type::SQ_CPPN){
        morphology = std::make_shared<sim::SQCPPNMorphology>(parameters);
        sq_cppn::cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_cppn();
        sq_t quadric = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_quadric();
        std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->set_cppn(cppn);
        std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->set_quadric(quadric);
    }
    else if(genome_type == morph_genome_type::SQ_CG){
        morphology = std::make_shared<sim::SQMorphology>(parameters);
        cg_t comp_gen = std::dynamic_pointer_cast<SQGenome>(morphGenome)->get_components_genome();
        sq_t quadric = std::dynamic_pointer_cast<SQGenome>(morphGenome)->get_quadric();
        std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->set_comp_gen(comp_gen);
        std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->set_quadric(quadric);
    }
    else if(genome_type == morph_genome_type::DUAL_CPPN){
        morphology = std::make_shared<sim::DualCPPNMorphology>(parameters);
        std::pair<skel_cppn_t,org_cppn_t> cppns = std::dynamic_pointer_cast<DualCPPNGenome>(morphGenome)->get_cppns();
        std::dynamic_pointer_cast<sim::DualCPPNMorphology>(morphology)->set_skel_cppn(cppns.first);
        std::dynamic_pointer_cast<sim::DualCPPNMorphology>(morphology)->set_org_cppn(cppns.second);
    }else{
        std::cerr << "Unknown type of morphological genome" << std::endl;
        std::cerr << "Possible values for parameter #morphGenomeType" << std::endl;
        std::cerr << "0: CPPN | 1: SQ_CPPN | 2: SQ_CG | 3: DUAL_CPPN" << std::endl;
        exit(1);
    }
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->set_morph_id(morphGenome->id());
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->set_randNum(randNum);
    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],0.15);



    feature_desc = std::dynamic_pointer_cast<sim::AREMorphology>(morphology)->getFeatureDesc();
    matrix_descriptor = std::dynamic_pointer_cast<sim::AREMorphology>(morphology)->getMatrixDesc();
    testRes = std::dynamic_pointer_cast<sim::AREMorphology>(morphology)->get_man_test_res().get_results();
    // matrix_4d = std::dynamic_pointer_cast<sim::AREMorphology>(morphology)->get_matrix_4d();
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
    int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;


    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<CPPNIndividual>();
    if(genome_type == morph_genome_type::CPPN)
        oarch.register_type<NN2CPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CPPN)
        oarch.register_type<SQCPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CG)
        oarch.register_type<SQGenome>();
    else if(genome_type == morph_genome_type::DUAL_CPPN)
        oarch.register_type<DualCPPNGenome>();
    oarch << *this;
    return sstream.str();
}

void CPPNIndividual::from_string(const std::string &str){
    int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;

    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CPPNIndividual>();
    if(genome_type == morph_genome_type::CPPN)
        iarch.register_type<NN2CPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CPPN)
        iarch.register_type<SQCPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CG)
        iarch.register_type<SQGenome>();
    else if(genome_type == morph_genome_type::DUAL_CPPN)
        iarch.register_type<DualCPPNGenome>();
    iarch >> *this;

    //set parameters and randNum to the genome as it is not contained in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}
