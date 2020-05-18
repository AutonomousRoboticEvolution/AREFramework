#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "CPPNGenome.h"
#include "ARE/Morphology_CPPNMatrix.h"
#include "v_repLib.h"
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
    Eigen::VectorXd getMorphDesc(){return morphDesc;};
    std::vector<bool> getManRes(){return testRes;};
    std::vector<std::vector<float>> getRawMat(){return rawMat;};
    std::vector<std::vector<float>> getProtoPhenotype(){return protoPhenotype;};
    double getManScore(){ return manScore;};
    void setGenome();
    void setMorphDesc();
    void setManRes();
    void setRawMat();
    void setProtoPhenotype();
    void setManScore();

    Eigen::VectorXd descriptor();

protected:
    void createController() override;
    void createMorphology() override;

    NEAT::NeuralNetwork nn;
    Eigen::VectorXd morphDesc;
    std::vector<bool> testRes;
    double manScore;
    std::vector<std::vector<float>> rawMat;
    std::vector<std::vector<float>> protoPhenotype;

};

}//are

#endif //CPPNINDIVIDUAL_H
