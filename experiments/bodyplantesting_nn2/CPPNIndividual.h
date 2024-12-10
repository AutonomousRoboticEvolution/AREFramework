#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "simulatedER/nn2/NN2CPPNGenome.hpp"
#include "simulatedER/nn2/sq_cppn_genome.hpp"
#include "ARE/Individual.h"
#include "ARE/misc/eigen_boost_serialization.hpp"
#include "ARE/learning/NSGA2.hpp"

namespace are {

class CPPNIndividual : public Individual, public NSGAIndividual<CPPNIndividual>
{
public :

    typedef std::shared_ptr<CPPNIndividual> Ptr;
    typedef std::shared_ptr<const CPPNIndividual> ConstPtr;

    CPPNIndividual() : Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const EmptyGenome::Ptr& ctrl_gen) :
            Individual(morph_gen, ctrl_gen){}
    CPPNIndividual(const CPPNIndividual& ind):
            Individual(ind),
            testRes(ind.testRes),
            feature_desc(ind.feature_desc),
            matrix_descriptor(ind.matrix_descriptor)
    {
        bool use_quadric = settings::getParameter<settings::Boolean>(ind.get_parameters(),"#useQuadric").value;
        if(use_quadric)
            morphGenome = std::make_shared<SQCPPNGenome>(*std::dynamic_pointer_cast<SQCPPNGenome>(ind.morphGenome));
        else
            morphGenome = std::make_shared<NN2CPPNGenome>(*std::dynamic_pointer_cast<NN2CPPNGenome>(ind.morphGenome));
        ctrlGenome = std::make_shared<EmptyGenome>(*std::dynamic_pointer_cast<EmptyGenome>(ind.ctrlGenome));
    }
    Individual::Ptr clone() override{
        return std::make_shared<CPPNIndividual>(*this);
    }

    void update(double) override{}

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & morphGenome;
        arch & feature_desc;
        arch & matrix_descriptor;
        arch & testRes;
        arch & individual_id;
        arch & generation;
    }
    // Serialization
    std::string to_string() const override;
    void from_string(const std::string &str) override;

    // Setters and getters
    std::vector<bool> getManRes(){return testRes;}



;
    /// Getters for descritors
    const sim::FeaturesDesc &get_feature_desc() const {return feature_desc;}
    std::vector<std::vector<double>> get_matrix_4d(){return matrix_4d;}
    Eigen::VectorXd descriptor() override;
    const sim::MatrixDesc& get_matrix_descriptor() const {return matrix_descriptor;}


    void set_morph_id(int id){morphGenome->set_id(id);}

protected:
    void createMorphology() override;
    void createController() override{}

    std::vector<bool> testRes;

    /// Descritors
    sim::FeaturesDesc feature_desc;
    sim::MatrixDesc matrix_descriptor;
    std::vector<std::vector<double>> matrix_4d;
};

}//are

#endif //CPPNINDIVIDUAL_H
