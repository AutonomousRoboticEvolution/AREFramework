#ifndef CPPNINDIVIDUAL_H
#define CPPNINDIVIDUAL_H

#include "simulatedER/nn2/NN2Individual.hpp"
#include "ARE/CPPNGenome.h"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/misc/eigen_boost_serialization.hpp"

namespace are {

class CPPNIndividual : public sim::NN2Individual
{
public :
    CPPNIndividual() : NN2Individual(){}
    CPPNIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
            NN2Individual(morph_gen, ctrl_gen){}
    CPPNIndividual(const CPPNIndividual& ind):
            NN2Individual(ind),
            nn(ind.nn),
            testRes(ind.testRes),
            morphDesc(ind.morphDesc),
            nn_inputs(ind.nn_inputs),
            nn_outputs(ind.nn_outputs)
    {}
    Individual::Ptr clone() override{
        return std::make_shared<CPPNIndividual>(*this);
    }

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & morphGenome;
        arch & final_position;
        arch & nn_inputs;
        arch & nn_outputs;

    }
    // Serialization
    std::string to_string();
    void from_string(const std::string &str);

    // Setters and getters
    NEAT::NeuralNetwork getGenome(){return nn;}

    std::vector<bool> getManRes(){return testRes;}
    void setGenome();

    void setManRes();

    /// Setters for descritors
    void setMorphDesc();
    void setGraphMatrix();
    void setSymDesc();
    void set_nn_inputs(int nni){nn_inputs = nni;}
    void set_nn_outputs(int nno){nn_outputs = nno;}
    /// Getters for descritors
    Eigen::VectorXd getMorphDesc(){return morphDesc;}
    NEAT::NeuralNetwork getGenome(){return nn;}
    std::vector<bool> getManRes(){return testRes;}

    Eigen::VectorXd descriptor();


    void update(double delta_time) override;

protected:
    void createMorphology() override;

    NEAT::NeuralNetwork nn;
    std::vector<bool> testRes;

    /// Descritors
    Eigen::VectorXd morphDesc;

private:

    int nn_inputs;
    int nn_outputs;
};

}//are

#endif //CPPNINDIVIDUAL_H
