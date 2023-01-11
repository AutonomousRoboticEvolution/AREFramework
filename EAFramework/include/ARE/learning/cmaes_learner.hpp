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

/**
 * @brief CMAESLearner Class:
 *      Wrapper class to use the ipop_cmaes algorithm implemented in libcmaes and interfaced in the ipop_cmaes class.
 *      This learner optimizes specificly the parameters of a neural network implemented with the nn2 library.
 */
class CMAESLearner : public Learner
{
public:

    struct novelty_params{
        static int k_value;
        static double novelty_thr;
        static double archive_adding_prob;
    };

    typedef std::shared_ptr<CMAESLearner> Ptr;
    typedef std::shared_ptr<const CMAESLearner> ConstPtr;

    using w_b_pair_t = std::pair<std::vector<double>,std::vector<double>>;
    typedef std::map<int,std::vector<IPOPCMAStrategy::individual_t>> archive_t;

    /**
     * @brief Default contstructor
     */
    CMAESLearner() : Learner(){}

    /**
     * @brief Constructor to build the class with the number of weights, biases, inputs and outputs of the neural network (controller).
     * The number of weights and biases fixes the size of the search space.
     * The number of inputs and outputs will be used to build a new controller when updated (see update_ctrl).
     * @param number of weights
     * @param number of biases
     * @param number of inputs
     * @param number of outputs
     */
    CMAESLearner(int nbr_weights, int nbr_biases, int nbr_inputs, int nbr_outputs){
        _dimension = nbr_weights + nbr_biases;
        _nbr_weights = nbr_weights;
        _nbr_biases = nbr_biases;
        _nn_inputs = nbr_inputs;
        _nn_outputs = nbr_outputs;
    }
    /**
     * @brief copy constructor
     * @param cl
     */
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

    /**
     * @brief initialize the learner
     * @param fitness target value
     * @param initial point in the search space. The mean of the normal distributionn will be set at this point
     */
    void init(double ftarget, std::vector<double> initial_point = std::vector<double>());

    /**
     * @brief step the algorithm:
     *      - increase the number of evaluation
     *      - if all the population has been evaluated do:
     *          - iterate the algo (see iterate)
     *          - archive the current population
     *          - generate next population (see next_pop)
     * @return true if the whole population has been evaluated false otherwise
     */
    bool step();

    void update(Control::Ptr &) override{}

    /**
     * @brief update the information of a specific individual just evaluated. This is done because the evaluation is done outside of this class
     * @param objective values
     * @param behavioral descriptor
     * @param trajectory
     */
    void update_pop_info(const std::vector<double>& obj, const Eigen::VectorXd &desc, std::vector<std::vector<waypoint> > &trajs);

    /**
     * @brief generate the next controller from the current population
     * @param Create a controller class of the next controller to be evaluated
     * @return return a pair of the weights and biases of the next controller
     */
    std::pair<std::vector<double>,std::vector<double>> update_ctrl(Control::Ptr& ctrl);

    /**
     * @brief set the random number generator. It is meant to be global in the experiment.
     * @param global random number gennerator
     */
    void set_randNum(const misc::RandNum::Ptr& rn){_rand_num = rn;}

    /**
     * @brief get the number of evaluations
     * @return number of evaluations
     */
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

    /**
     * @brief produce a string from the archived populations
     * @return return the string
     */
    std::string archive_to_string() const;
    /**
     * @brief set the number of dropped evaluations.
     * @param number of dropped evaluations
     */
    void set_nbr_dropped_eval(const int& nde){nbr_dropped_eval = nde;}
    /**
     * @brief get the best solution of all the learning
     * @return
     */
    const IPOPCMAStrategy::individual_t& get_best_solution() const {return _best_solution;}
    /**
     * @brief get the current population
     * @return
     */
    const std::vector<IPOPCMAStrategy::individual_t>& get_population() const {return _cma_strat->get_population();}

    /**
     * @brief compute the learning process
     * @return
     */
    double learning_progress() const {return _cma_strat->learning_progress();}
    /**
     * @brief true if the learning is finished
     * @return
     */
    bool is_learning_finish() const;

    /**
     * @brief get a vector of pair of weights and biases representing the next population. Can be called only once per iteration
     * @return
     */
    std::vector<w_b_pair_t> get_new_population();
    /**
     * @brief get the remaining population to be evaluated in the form of a vector of pair of weights and biases. Can be called several times per iteration
     * @return
     */
    std::vector<w_b_pair_t> get_remaining_population();

    /**
     * @brief true if the learner as been initialized.
     * @return
     */
    bool is_initialized() const {return initialized;}

    /**
     * @brief  true if both the learner as been initialized and has finished. Used with a morpho-generative algorithm (mnipes v2)
     */
    void to_be_erased() {initialized = true; _is_finish = true;}

    /**
     * @brief number evaluation processed by the learner so far
     * @return
     */
    int nbr_eval() const{return _nbr_eval;}

    int get_nbr_inputs() const{return _nn_inputs;}
    int get_nbr_outputs() const{return _nn_outputs;}
    int get_nbr_weights() const{return _nbr_weights;}
    int get_nbr_biases() const{return _nbr_biases;}

    /**
     * @brief number iteration done
     * @return
     */
    int nbr_iter() const{return _generation;}

    const archive_t &get_archive() const{return _archive;}

    /**
     * @brief produce a human readable string describing the current state of the learner.
     * @return
     */
    std::string print_info() const{return _cma_strat->print_info();}

    /**
     * @brief serializes the class in form of a string
     * @return
     */
    std::string to_string() const;
    /**
     * @brief deserializes the information contained in a string
     */
    void from_string(const std::string&);
    /**
     * @brief deserializes a file using from_string
     */
    void from_file(const std::string&);


protected:
    /**
     * @brief generate the next population
     */
    void next_pop();
    /**
     * @brief iterate the learner:
     *  - Compute the novelty score of all the individual
     *  - Attribute the objective value for CMAES update by combining the novelty score and goal-based fitness with a weighted sum
     *  - Update the covariance matrix of CMAES
     *  - Evaluate the stoping and restarting criteria
     */
    void iterate();

    int _dimension; //!< dimensionality of the search space
    IPOPCMAStrategy::Ptr _cma_strat; //!< IPOP CMAES class containing the algorithm
    IPOPCMAStrategy::individual_t _best_solution; //!< the best solution found so far
    misc::RandNum::Ptr _rand_num; //!< random number solution generator (should global to the experiment)
    std::vector<Eigen::VectorXd> _population; //!< current population
    int _nn_inputs; //!< input layer size of the neural network being optimize
    int _nn_outputs; //!< output layer size of the neural network being optimize
    int _nbr_biases; //!< number of biases of the neural network being optimize
    int _nbr_weights; //!< number of weights size of the neural network being optimize
    int _nbr_eval = 0; //!< current number of evaluations
    int _generation = 0; //!< current number of iterations/generations
    bool _is_finish = false; //!< is the learning finished?
    archive_t _archive; //!< list of all the individual produced so far organized by iteration
    std::vector<Eigen::VectorXd> _novelty_archive; //!< archive used by the novelty search
    int nbr_dropped_eval = 0; //!< number of evaluations dropped so far
    int current_nbr_ind = 0; //!< number of individual left to evaluate
    bool new_population_available = true; //!< Is their a new population available to be retrieved? Used by get_new_population.
    bool from_scratch=true; //!< Is learning starting from a random point in  the search space?

    bool initialized = false; //!< Is the learner initialized?
};

}//are

#endif //CMAES_LEARNER_HPP
