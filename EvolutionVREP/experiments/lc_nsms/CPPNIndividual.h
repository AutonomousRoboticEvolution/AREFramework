#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "ARE/Individual.h"
#include "ARE/CPPNGenome.h"
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

    std::vector<bool> getManRes(){return testRes;};
    std::vector<std::vector<float>> getRawMat(){return rawMat;};
    double getManScore(){ return manScore;};
    void setGenome();

    void setManRes();
    void setRawMat();
    void setManScore();

    /// Setters for descritors
    void setMorphDesc();
    void setGraphMatrix();
    void setCPPNBoolSkeletonMatrix();
    void setCPPNBoolWheelMatrix();
    void setCPPNBoolSensorMatrix();
    void setCPPNBoolJointMatrix();
    void setCPPNBoolCasterMatrix();
    void setInterWheelMatrix();
    void setInterSensorMatrix();
    void setInterJointMatrix();
    void setInterCasterMatrix();
    /// Getters for descritors
    Eigen::VectorXd getMorphDesc(){return morphDesc;};
    std::vector<std::vector<std::vector<int>>> getGraphMatrix(){return graphMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolSkeletonMatrix(){return skeletonMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolWheelMatrix(){return wheelMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolSensorMatrix(){return sensorMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolJointMatrix(){return jointMatrix;};
    std::vector<std::vector<std::vector<bool>>> getCPPNBoolCasterMatrix(){return casterMatrix;};
    std::vector<std::vector<std::vector<bool>>> getInterWheelMatrix(){return wheelInter;};
    std::vector<std::vector<std::vector<bool>>> getInterSensorMatrix(){return sensorInter;};
    std::vector<std::vector<std::vector<bool>>> getInterJointMatrix(){return jointInter;};
    std::vector<std::vector<std::vector<bool>>> getInterCasterMatrix(){return casterInter;};

    Eigen::VectorXd descriptor();

protected:
    void createController() override;
    void createMorphology() override;

    NEAT::NeuralNetwork nn;
    std::vector<bool> testRes;
    double manScore;
    std::vector<std::vector<float>> rawMat;

    /// Descritors
    std::vector<std::vector<std::vector<int>>> graphMatrix;
    Eigen::VectorXd morphDesc;
    std::vector<std::vector<std::vector<bool>>> skeletonMatrix;
    std::vector<std::vector<std::vector<bool>>> wheelMatrix;
    std::vector<std::vector<std::vector<bool>>> sensorMatrix;
    std::vector<std::vector<std::vector<bool>>> jointMatrix;
    std::vector<std::vector<std::vector<bool>>> casterMatrix;
    std::vector<std::vector<std::vector<bool>>> wheelInter;
    std::vector<std::vector<std::vector<bool>>> sensorInter;
    std::vector<std::vector<std::vector<bool>>> jointInter;
    std::vector<std::vector<std::vector<bool>>> casterInter;
};

}//are

#endif //CPPNINDIVIDUAL_H
