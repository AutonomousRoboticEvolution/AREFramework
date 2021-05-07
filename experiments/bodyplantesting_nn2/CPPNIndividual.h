#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/nn2/NN2CPPNGenome.hpp"
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
    CPPNIndividual(const NN2CPPNGenome::Ptr& morph_gen,const EmptyGenome::Ptr& ctrl_gen) :
            Individual(morph_gen, ctrl_gen){}
    CPPNIndividual(const CPPNIndividual& ind):
            Individual(ind),
            testRes(ind.testRes),
            morphDesc(ind.morphDesc)
    {}
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
    }
    // Serialization
    std::string to_string();
    void from_string(const std::string &str);

    // Setters and getters
    std::vector<bool> getManRes(){return testRes;}


    void setManRes();

    /// Setters for descritors
    void setMorphDesc();
    void setGraphMatrix();
    void setSymDesc();
    /// Getters for descritors
    Eigen::VectorXd getMorphDesc(){return morphDesc;}

    Eigen::VectorXd descriptor();

protected:
    void createMorphology() override;
    void createController() override{}

    std::vector<bool> testRes;

    /// Descritors
    Eigen::VectorXd morphDesc;

};

}//are

#endif //CPPNINDIVIDUAL_H
