#ifndef CMAES_LEARNER_HPP
#define CMAES_LEARNER_HPP

#include "ARE/Learner.h"
#include "ARE/learning/ipop_cmaes.hpp"
#include "ARE/learning/Novelty.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/misc/RandNum.h"
#include "ARE/misc/eigen_boost_serialization.hpp"


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

    using w_b_pair_t = std::pair<std::vector<double>,std::vector<double>>;
    typedef std::map<int,std::vector<IPOPCMAStrategy::individual_t>> archive_t;

    CMAESLearner() : Learner(){}
    CMAESLearner(int nbr_weights, int nbr_biases, int nbr_inputs, int nbr_outputs){
        _dimension = nbr_weights + nbr_biases;
        _nbr_weights = nbr_weights;
        _nbr_biases = nbr_biases;
        _nn_inputs = nbr_inputs;
        _nn_outputs = nbr_outputs;
    }
    CMAESLearner(const CMAESLearner& cl) : Learner(cl),
        _dimension(cl._dimension),
        _cma_strat(cl._cma_strat),
        _best_solution(cl._best_solution),
        _rand_num(cl._rand_num),
        _population(cl._population),
        _nn_inputs(cl._nn_inputs),
        _nn_outputs(cl._nn_outputs),
        _nbr_biases(cl._nbr_biases),
        _nbr_weights(cl._nbr_weights),
        _nbr_eval(cl._nbr_eval),
        _generation(cl._generation),
        _is_finish(cl._is_finish),
        _archive(cl._archive),
        _novelty_archive(cl._novelty_archive),
        nbr_dropped_eval(cl.nbr_dropped_eval),
        initialized(cl.initialized),
        current_nbr_ind(cl.current_nbr_ind){}

    void init(double ftarget, std::vector<double> initial_point = std::vector<double>());


    bool step();

    void update(Control::Ptr &) override{}

    void update_pop_info(const std::vector<double>& obj, const Eigen::VectorXd &desc = Eigen::VectorXd::Zero(1));
    std::pair<std::vector<double>,std::vector<double>> update_ctrl(Control::Ptr&);

    void set_randNum(const misc::RandNum::Ptr& rn){_rand_num = rn;}
    int get_nbr_eval(){return _nbr_eval;}

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Learner>(*this);
        arch & _archive;
        arch & _best_solution;
        arch & _cma_strat;
        arch & _nn_inputs;
        arch & _nn_outputs;
        arch & _nbr_biases;
        arch & _nbr_weights;
        arch & _nbr_eval;
        arch & _generation;
        arch & _is_finish;
        arch & _novelty_archive;
        arch & _population;
        arch & _best_solution;
        arch & nbr_dropped_eval;
        arch & current_nbr_ind;
    }

    std::string archive_to_string() const;
    void set_nbr_dropped_eval(const int& nde){nbr_dropped_eval = nde;}
    const std::pair<double,std::vector<double>>& get_best_solution() const {return _best_solution;}
    const std::vector<IPOPCMAStrategy::individual_t>& get_population() const {return _cma_strat->get_population();}
    double learning_progress() const {return _cma_strat->learning_progress();}
    bool is_learning_finish() const;

    std::vector<w_b_pair_t> get_new_population();
    std::vector<w_b_pair_t> get_remaining_population();

    bool is_initialized() const {return initialized;}

    void to_be_erased() {initialized = true; _is_finish = true;}

    int nbr_eval(){return _nbr_eval;}

    int get_nbr_inputs() const{return _nn_inputs;}
    int get_nbr_outputs() const{return _nn_outputs;}
    int get_nbr_weights() const{return _nbr_weights;}
    int get_nbr_biases() const{return _nbr_biases;}

    int nbr_iter() const{return _generation;}
    int nbr_eval() const{return _nbr_eval;}

    std::string print_info() const{return _cma_strat->print_info();}

    std::string to_string() const;
    void from_string(const std::string&);
    void from_file(const std::string&);


protected:
    void next_pop();
    void iterate();

    int _dimension;
    IPOPCMAStrategy::Ptr _cma_strat;
    std::pair<double,std::vector<double>> _best_solution;
    misc::RandNum::Ptr _rand_num;
    std::vector<Eigen::VectorXd> _population;
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
    int current_nbr_ind = 0;
    bool new_population_available = true;
    bool from_scratch=true;

    bool initialized = false;
};

}//are

#endif //CMAES_LEARNER_HPP
