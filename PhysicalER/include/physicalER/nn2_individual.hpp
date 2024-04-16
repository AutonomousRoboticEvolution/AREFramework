#ifndef PI_INDIVIDUAL_HPP
#define PI_INDIVIDUAL_HPP

#include "controller.hpp"
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "nn2/mlp.hpp"
#include "nn2/rnn.hpp"
#include "nn2/elman.hpp"
#include "nn2/fcp.hpp"

namespace are{

namespace phy{

using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using connection_t = nn2::Connection<double>;
using ffnn_t = nn2::Mlp<neuron_t,connection_t>;
using elman_t = nn2::Elman<neuron_t,connection_t>;
using rnn_t = nn2::Rnn<neuron_t,connection_t>;
using fcp_t = nn2::Fcp<neuron_t,connection_t>;

/**
 * @brief class Individual meant to be running on a Raspberry Pi using NN implementation of nn2 module
 */
class NN2Individual : public Controller
{
public:

    typedef std::shared_ptr<NN2Individual> Ptr;
    typedef std::shared_ptr<const NN2Individual> ConstPtr;

    NN2Individual() : Controller(){}
    NN2Individual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
        Controller(morph_gen,ctrl_gen){}
    NN2Individual(const NN2Individual& ind) :
        Controller(ind),
        energy_cost(ind.energy_cost)
    {}

    Individual::Ptr clone() override {
        return std::make_shared<NN2Individual>(*this);
    }

    void update(double delta_time) override;

    std::string to_string();
    void from_string(const std::string&);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & nb_input;
        arch & nb_output;
        arch & nb_hidden;
        arch & nn_type;
        arch & energy_cost;
    }

    //GETTERS & SETTERS



protected:
    void createController() override;

    int nb_hidden;
    int nn_type;

    double energy_cost=0;
};



}//pi
}//are
#endif //PI_INDIVIDUAL_HPP
