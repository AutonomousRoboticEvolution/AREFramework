#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ProtomatrixGenome.hpp"
#include "ARE/Individual.h"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/misc/eigen_boost_serialization.hpp"
#include "ARE/learning/NSGA2.hpp"

namespace are {

class CPPNIndividual : public Individual, public NSGAIndividual<CPPNIndividual>
{
public :

    typedef std::shared_ptr<CPPNIndividual> Ptr;
    typedef std::shared_ptr<const CPPNIndividual> ConstPtr;

    CPPNIndividual() : Individual(){}
    CPPNIndividual(const ProtomatrixGenome::Ptr& morph_gen,const EmptyGenome::Ptr& ctrl_gen) :
            Individual(morph_gen, ctrl_gen){}
    CPPNIndividual(const CPPNIndividual& ind):
            Individual(ind),
            testRes(ind.testRes),
            morphDesc(ind.morphDesc)
    {
        morphGenome = std::make_shared<ProtomatrixGenome>(*std::dynamic_pointer_cast<ProtomatrixGenome>(ind.morphGenome));
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
        arch & morphDesc;
        arch & testRes;
        arch & individual_id;
        arch & generation;
    }
    // Serialization
    std::string to_string();
    void from_string(const std::string &str);

    // Setters and getters
    std::vector<bool> getManRes(){return testRes;}


    void setManRes();

    /// Setters for descritors
    void setMorphDesc();
    void set_4d_matrix();
    /// Getters for descritors
    CartDesc getMorphDesc(){return morphDesc;}
    std::vector<std::vector<double>> get_matrix_4d(){return matrix_4d;};
    Eigen::VectorXd descriptor();
    const OrganPositionDesc& get_organ_position_descriptor() const {return organ_position_descriptor;}
    void set_organ_position_descriptor();

protected:
    void createMorphology() override;
    void createController() override{}

    std::vector<bool> testRes;

    /// Descritors
    CartDesc morphDesc;
    OrganPositionDesc organ_position_descriptor;
    std::vector<std::vector<double>> matrix_4d;
};

}//are

#endif //CPPNINDIVIDUAL_H
