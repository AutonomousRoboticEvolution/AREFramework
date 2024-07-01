#ifndef BOINDIVIDUAL_H
#define BOINDIVIDUAL_H

#include "ARE/Individual.h"
#include "simulatedER/FixedMorphology.hpp"
#include "BayesianOpt.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "ARE/Settings.h"
#include "ARE/nn2/NN2Settings.hpp"
#include "simulatedER/mazeEnv.h"
#include "nn2/mlp.hpp"
#include "nn2/elman.hpp"
#include "nn2/rnn.hpp"


namespace are {

using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using connection_t = nn2::Connection<double>;
using ffnn_t = nn2::Mlp<neuron_t,connection_t>;
using elman_t = nn2::Elman<neuron_t,connection_t>;
using rnn_t = nn2::Rnn<neuron_t,connection_t>;


typedef enum DescriptorType{
    FINAL_POSITION = 0,
    VISITED_ZONES = 1
}DescriptorType;

class BOIndividual : public sim::NN2Individual
{
public :

    typedef std::shared_ptr<BOIndividual> Ptr;
    typedef std::shared_ptr<const BOIndividual> ConstPtr;

    BOIndividual() : sim::NN2Individual(){}
    BOIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen);
    BOIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen, const BOLearner::Ptr& l);
    BOIndividual(const BOIndividual& ind) :
        sim::NN2Individual(ind),
        init_position(ind.init_position)
    {}

    Individual::Ptr clone();


    void compute_model(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl);
    void update_learner(std::vector<Eigen::VectorXd> &obs, std::vector<Eigen::VectorXd> &spl, const Eigen::VectorXd &target);
    const Learner::Ptr &getLearner(){return learner;}

    void set_final_position(const std::vector<double> fp){final_position = fp;}
    const std::vector<double> get_final_position(){return final_position;}

    void set_trajectory(const std::vector<waypoint> &traj){trajectory = traj;}
    const std::vector<waypoint> &get_trajectory(){return trajectory;}

    int genType = 2;

    std::string to_string();
    void from_string(const std::string&);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
        arch & trajectory;
    }
    Eigen::VectorXd descriptor() override;

    void set_visited_zones(const Eigen::MatrixXi& vz){visited_zones = vz;}
    void set_descriptor_type(DescriptorType dt){descriptor_type = dt;}


protected:
    std::vector<double> init_position;

    Eigen::MatrixXi visited_zones;
    DescriptorType descriptor_type = FINAL_POSITION;
};

}//are

#endif //BOINDIVIDUAL_H

