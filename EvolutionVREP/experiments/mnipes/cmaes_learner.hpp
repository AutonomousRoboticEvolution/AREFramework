#ifndef CMAES_LEARNER_HPP
#define CMAES_LEARNER_HPP

#include "ARE/Learner.h"
#include "ARE/learning/NIPES.hpp"
#include "misc/RandNum.h"

namespace are {

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

    void init();
    void next_pop();
    void iterate();
    bool step();

    void update(Control::Ptr&) override;

    void update_pop_info(const std::vector<double>& obj, const Eigen::VectorXd &desc = Eigen::VectorXd::Zero(1));

    void set_randNum(const misc::RandNum::Ptr& rn){_rand_num = rn;}
    int get_nbr_eval(){return _nbr_eval;}

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Learner>(*this);
        arch & _archive;
    }

    std::string archive_to_string();

protected:
    int _dimension;
    IPOPCMAStrategy::Ptr _cma_strat;
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
};

}//are

#endif //CMAES_LEARNER_HPP
