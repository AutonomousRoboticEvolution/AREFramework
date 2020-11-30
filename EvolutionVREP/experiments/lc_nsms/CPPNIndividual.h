#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "ARE/CPPNGenome.h"
#include "ARE/Morphology_CPPNMatrix.h"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include "eigen_boost_serialization.hpp"

namespace are {

class CPPNIndividual : public Individual
{
public :
    CPPNIndividual() : Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen);
//    CPPNIndividual(const CPPNIndividual& ind) :
//            Individual(ind), nn(ind.nn), morphDesc(ind.morphDesc), testRes(ind.testRes){}

    Individual::Ptr clone();

    void update(double delta_time);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & morphGenome;
        arch & morphDesc;
        arch & testRes;
        arch & manScore;
    }
    // Serialization
    std::string to_string();
    void from_string(const std::string &str);

    // Setters and getters
    NEAT::NeuralNetwork getGenome(){return nn;};

    std::vector<bool> getManRes(){return testRes;};
    double getManScore(){ return manScore;};
    void setGenome();

    void setManRes();
    void setManScore();

    /// Setters for descritors
    void setMorphDesc();
    void setGraphMatrix();
    void setSymDesc();
    /// Getters for descritors
    Eigen::VectorXd getMorphDesc(){return morphDesc;};
    std::vector<std::vector<std::vector<int>>> getGraphMatrix(){return graphMatrix;};
    Eigen::VectorXd getSymDesc(){return symDesc;};

protected:
    void createController() override;
    void createMorphology() override;

    NEAT::NeuralNetwork nn;
    std::vector<bool> testRes;
    double manScore;

    /// Descritors
    std::vector<std::vector<std::vector<int>>> graphMatrix;
    Eigen::VectorXd morphDesc;
    Eigen::VectorXd symDesc;
};

}//are

#endif //CPPNINDIVIDUAL_H
