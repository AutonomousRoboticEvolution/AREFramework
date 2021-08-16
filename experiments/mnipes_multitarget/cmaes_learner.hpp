#ifndef CMAES_LEARNER_HPP
#define CMAES_LEARNER_HPP

#include <cmath>

#include "ARE/Learner.h"
#include "ARE/learning/ipop_cmaes.hpp"
#include "ARE/learning/Novelty.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/misc/RandNum.h"

namespace are {

using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using connection_t = nn2::Connection<double>;
using ffnn_t = nn2::Mlp<neuron_t,connection_t>;
using elman_t = nn2::Elman<neuron_t,connection_t>;
using rnn_t = nn2::Rnn<neuron_t,connection_t>;

class CMAESLearner : public Learner
{
public:
    typedef std::shared_ptr<CMAESLearner> Ptr;
    typedef std::shared_ptr<const CMAESLearner> ConstPtr;

    typedef std::map<int,std::vector<IPOPCMAStrategy::individual_t>> archive_t;

    CMAESLearner() : Learner(){}
    CMAESLearner(int nbr_weights, int nbr_biases, int nbr_inputs, int nbr_outputs){
        _dimension = nbr_weights + nbr_biases;
        _nbr_weights = nbr_weights;
        _nbr_biases = nbr_biases;
        _nn_inputs = nbr_inputs;
        _nn_outputs = nbr_outputs;
    }

    void init(std::vector<double> initial_point = std::vector<double>());
    void next_pop();
    void iterate();
    bool step();

    void update(Control::Ptr &) override{}
    std::pair<std::vector<double>,std::vector<double>> update_ctrl(Control::Ptr&);

    void update_pop_info(const std::vector<double>& obj, const Eigen::VectorXd &desc = Eigen::VectorXd::Zero(1));

    void set_randNum(const misc::RandNum::Ptr& rn){_rand_num = rn;}
    int get_nbr_eval(){return _nbr_eval;}

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Learner>(*this);
        arch & _archive;
        arch & _best_solution;
    }

    std::string archive_to_string();
    void set_nbr_dropped_eval(const int& nde){nbr_dropped_eval = nde;}
    const std::pair<double,std::vector<double>>& get_best_solution(){return _best_solution;}
    const std::vector<IPOPCMAStrategy::individual_t>& get_population(){return _cma_strat->get_population();}
    double learning_progress(){return _cma_strat->learning_progress();}

protected:
    int _dimension;
    IPOPCMAStrategy::Ptr _cma_strat;
    std::pair<double,std::vector<double>> _best_solution;
    misc::RandNum::Ptr _rand_num;
    std::vector<Eigen::VectorXd> _population;
    int _counter = 0;
    int _nn_inputs;
    int _nn_outputs;
    int _nbr_biases;
    int _nbr_weights;
    int _nbr_eval = 0;
    int _generation = 0;
    bool _is_finish = false;
    archive_t _archive;
    std::vector<Eigen::VectorXd> _novelty_archive;
    int nbr_dropped_eval = 0;
};

}//are

#endif //CMAES_LEARNER_HPP
