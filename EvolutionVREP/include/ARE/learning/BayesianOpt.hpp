#ifndef BOLEARNER_H
#define BOLEARNER_H

#include <tbb/tbb.h>
#include <Eigen/Core>

#define USE_LIBCMAES
#include <limbo/limbo.hpp>

#include "are_limbo/gp_ucb.hpp"

#include "ARE/Learner.h"
#include "ARE/NNParamGenome.hpp"
#include "ARE/Settings.h"

namespace lb = limbo;
namespace arelb = are_limbo;
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
        BO_PARAM(double, jitter, 0.1);
    };
    struct acqui_ucb {
        BO_PARAM(double, alpha, 0.1);
    };
    struct acqui_gpucb {
        BO_DYN_PARAM(double, delta);
    };
    struct init_randomsampling {
        BO_PARAM(int, samples, 10);
    };
    struct kernel : public lb::defaults::kernel {
        BO_PARAM(double, noise, 1e-10);
    };

    struct kernel_maternfivehalves : public lb::defaults::kernel_maternfivehalves {
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


using kernel_t = lb::kernel::MaternFiveHalves<Params>;
using mean_t = lb::mean::Data<Params>;
using gp_opt_t = lb::model::gp::KernelLFOpt<Params>;

using gp_t = lb::model::GP<Params, kernel_t, mean_t, gp_opt_t>;
//using multi_gp_t = lb::model::MultiGP<Params, lb::model::GP, kernel_t, mean_t, gp_opt_t>;

//    using policy_opt_t = lb::opt::Cmaes<Params>;

using acqui_t = arelb::acqui::GP_UCB<Params, gp_t>;
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
    BOLearner(const settings::ParametersMapPtr &param);
    void update(Control::Ptr &ctrl){}
    void update(const NNParamGenome::Ptr &ctrl_gen);
    void init_model(int input_size);
    void compute_model();
    void update_model();

    double get_sigma(const Eigen::VectorXd& x){return _model.sigma(x);}
    double get_sigma(const std::vector<double>& x){
        Eigen::VectorXd X(x.size());
        for(int i = 0; i < x.size(); i++)
            X(i) = x[i];
        return _model.sigma(X);
    }

    Eigen::VectorXd get_mu(const Eigen::VectorXd& x){return _model.mu(x);}
    Eigen::VectorXd get_mu(const std::vector<double>& x){
        Eigen::VectorXd X(x.size());
        for(int i = 0; i < x.size(); i++)
            X(i) = x[i];
        return _model.mu(X);
    }

    double reward(const Eigen::VectorXd& x){
        return _reward(x);
    }

    //GETTERS & SETTERS
    model_t get_model(){return _model;}
    void set_observation(std::vector<Eigen::VectorXd> &obs){_observations = obs;}
    void set_samples(std::vector<Eigen::VectorXd> &s){_samples = s;}
    int dataset_size(){return _samples.size();}
    const model_t& model() const {return _model;}
    void set_target(const Eigen::VectorXd& target){_target = target;}

private:
    model_t _model;
    Eigen::VectorXd _target;
    std::function<double(const Eigen::VectorXd& x)> _reward;
    double _max_dist;
};
}//are
#endif //BOLEARNER_H
