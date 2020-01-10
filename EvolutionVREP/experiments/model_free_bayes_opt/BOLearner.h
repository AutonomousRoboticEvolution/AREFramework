#ifndef BOLEARNER_H
#define BOLEARNER_H

#include <tbb/tbb.h>
#include <Eigen/Core>

#define USE_LIBCMAES
#include <limbo/limbo.hpp>

#include "ARE/Learner.h"
#include "NNControl.h"

namespace lb = limbo;
namespace are {
struct Params {
    struct bayes_opt_boptimizer : public lb::defaults::bayes_opt_boptimizer {
        BO_PARAM(int, hp_period, 10);
    };
    struct bayes_opt_bobase : public lb::defaults::bayes_opt_bobase {
        BO_PARAM(int, stats_enabled, true);
    };
    struct stop_maxiterations {
        BO_PARAM(int, iterations, 10);
    };
    struct stop_mintolerance {
        BO_PARAM(double, tolerance, -0.1);
    };
    struct acqui_ei {
        BO_PARAM(double, jitter, 10.0);
    };
    struct acqui_ucb {
 	 BO_PARAM(double, alpha, 0.1);
    };
    struct acqui_gpucb {
        BO_PARAM(double, delta, 0.1);
    };
    struct init_randomsampling {
        BO_PARAM(int, samples, 10);
    };
    struct kernel : public lb::defaults::kernel {
        BO_PARAM(double, noise, 1e-10);
    };

    struct kernel_squared_exp_ard : public lb::defaults::kernel_squared_exp_ard {
    };
    struct opt_rprop : public lb::defaults::opt_rprop {
    };
    struct opt_parallelrepeater : public lb::defaults::opt_parallelrepeater {
    };
    struct opt_cmaes : public lb::defaults::opt_cmaes {
        BO_DYN_PARAM(int, max_fun_evals);
        BO_DYN_PARAM(double, fun_tolerance);
        BO_DYN_PARAM(int, restarts);
        BO_DYN_PARAM(int, elitism);
        BO_DYN_PARAM(bool, handle_uncertainty);

        BO_DYN_PARAM(int, lambda);

        BO_PARAM(int, variant, aIPOP_CMAES);
        BO_PARAM(int, verbose, false);
        BO_PARAM(bool, fun_compute_initial, true);
        // BO_PARAM(double, fun_target, 30);
        BO_DYN_PARAM(double, ubound);
        BO_DYN_PARAM(double, lbound);
    };
};

//template <typename Params>
//struct DistanceToTarget {
//    using result_type = double;
//    DistanceToTarget(const Eigen::Vector2d& target) : _target(target) {}

//    double operator()(const Eigen::VectorXd& x) const
//    {
//        return -(x - _target).norm();
//    }

//protected:
//    Eigen::Vector2d _target;
//};


using kernel_t = lb::kernel::SquaredExpARD<Params>;
using mean_t = lb::mean::Data<Params>;
using gp_opt_t = lb::model::gp::KernelLFOpt<Params>;

using gp_t = lb::model::GP<Params, kernel_t, mean_t, gp_opt_t>;

//    using policy_opt_t = lb::opt::Cmaes<Params>;

using acqui_t = lb::acqui::GP_UCB<Params, gp_t>;
using acqui_opt_t = lb::opt::Cmaes<Params>;
using init_t = lb::init::NoInit<Params>;
using stop_t = lb::stop::MaxIterations<Params>;

using stat_t = lb::stat::GP<Params>;

class BOLearner :
        public Learner,
        public lb::bayes_opt::BoBase<Params,
        lb::modelfun<gp_t>,
        lb::acquifun<acqui_t>,
        lb::acquiopt<acqui_opt_t>,
        lb::initfun<init_t>,
        lb::statsfun<stat_t>,
        lb::stopcrit<stop_t>>
{
public:

    typedef std::shared_ptr<BOLearner> Ptr;
    typedef std::shared_ptr<const BOLearner> ConstPtr;

    BOLearner();
    void update(Control::Ptr &ctrl);
    void init_model(int input_size);
    void update_model();


    //GETTERS & SETTERS
    model_t get_model(){return _model;}
    void set_observation(std::vector<Eigen::VectorXd> &obs){_observations = obs;}
    void set_samples(std::vector<Eigen::VectorXd> &s){_samples = s;}
    int dataset_size(){return _samples.size();}
    const model_t& model() const {return _model;}

private:
    model_t _model;
    Eigen::VectorXd _initial_pos;
};
}//are
#endif //BOLEARNER_H
