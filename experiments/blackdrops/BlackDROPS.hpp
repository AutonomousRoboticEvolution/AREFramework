#ifndef BOLEARNER_H
#define BOLEARNER_H

#include <tbb/tbb.h>
#include <Eigen/Core>

#include <limbo/kernel/squared_exp_ard.hpp>
#include <limbo/mean/constant.hpp>
#include <limbo/model/gp.hpp>
#include <limbo/model/multi_gp.hpp>
#include <limbo/model/multi_gp/parallel_lf_opt.hpp>
#define USE_LIBCMAES
#include <limbo/opt/cmaes.hpp>

#include <blackdrops/blackdrops.hpp>
#include <blackdrops/model/gp/kernel_lf_opt.hpp>
#include <blackdrops/model/gp_model.hpp>
#include <blackdrops/system/system.hpp>

#include <blackdrops/policy/nn_policy.hpp>

#include <blackdrops/reward/reward.hpp>

#include <blackdrops/utils/cmd_args.hpp>
#include <blackdrops/utils/utils.hpp>

#include "ARE/Learner.h"
#include "NNControl.h"

namespace lb = limbo;
namespace bd = blackdrops;

struct Params {
    struct blackdrops : public ::bd::defaults::blackdrops {
        BO_PARAM(size_t, action_dim, 2); // @action_dim - here you should fill the dimensions of the action space
        BO_PARAM(size_t, model_input_dim, 8); // @transformed_state - here you should fill the input dimensions to the GPs and the policy
        BO_PARAM(size_t, model_pred_dim, 8); // @state_dim - here you should fill the actual dimensions of the state
        BO_PARAM(double, dt, 1.); // @dt - here you should fill the sampling step
        BO_PARAM(double, T, 1.); // @T - here you should fill the duration time of each episode/trial
        BO_DYN_PARAM(bool, verbose);
        BO_DYN_PARAM(bool, stochastic);
        BO_DYN_PARAM(double, boundary);
    };

    struct gp_model {
        BO_PARAM(double, noise, 0.01);
    };

    struct mean_constant {
        BO_PARAM(double, constant, 0.0);
    };

    struct kernel : public lb::defaults::kernel {
        BO_PARAM(double, noise, gp_model::noise());
        BO_PARAM(bool, optimize_noise, true);
    };

    struct kernel_squared_exp_ard : public lb::defaults::kernel_squared_exp_ard {
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

    struct opt_rprop : public lb::defaults::opt_rprop {
        BO_PARAM(int, iterations, 300);
        BO_PARAM(double, eps_stop, 1e-4);
    };
};

struct PolicyParams {
    struct blackdrops : public Params::blackdrops {
    };

    struct nn_policy {
        BO_PARAM(size_t, state_dim, Params::blackdrops::model_input_dim());
        BO_PARAM(size_t, action_dim, Params::blackdrops::action_dim());
        BO_PARAM_ARRAY(double, max_u, 1., 1.); // @max_action - here you should fill the absolute max value for each action dimension
        BO_DYN_PARAM(int, hidden_neurons);
        BO_PARAM_ARRAY(double, limits, 1., 1., 1., 1., 1., 1.,1.,1.); // @normalization_factor - here you should fill the normalization factors for the inputs of the policy (absolute values)
        BO_PARAM(double, af, 1.0);
    };
};

struct ARESystem :
        public bd::system::System<Params,ARESystem, bd::RolloutInfo>
{
// Assuming an empty system is enough as the simulation is handled by the ARE framework
//    Eigen::VectorXd init_state() const;
//    Eigen::VectorXd transform_state(const Eigen::VectorXd& original_state) const;
//    Eigen::VectorXd add_noise(const Eigen::VectorXd &original_state) const;
//    Eigen::VectorXd policy_transform(const Eigen::VectorXd &original_state, blackdrops::RolloutInfo *info) const;
};

struct RewardFunction : public bd::reward::Reward<RewardFunction> {
    Eigen::VectorXd _target;

    template <typename RolloutInfo>
    double operator()(const RolloutInfo& info, const Eigen::VectorXd& from_state, const Eigen::VectorXd& action, const Eigen::VectorXd& to_state) const
    {
        return (to_state - _target).norm();
    }
};


namespace are {

using kernel_t = lb::kernel::SquaredExpARD<Params>;
using mean_t = lb::mean::Constant<Params>;
using GP_t = lb::model::MultiGP<Params, limbo::model::GP, kernel_t, mean_t, limbo::model::multi_gp::ParallelLFOpt<Params, blackdrops::model::gp::KernelLFOpt<Params>>>;

using policy_opt_t = lb::opt::Cmaes<Params>;

using MGP_t = bd::model::GPModel<Params, GP_t>;

using obs_t = std::vector<std::tuple<Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd>>;

class BDROPS :
        public Learner,
        public bd::BlackDROPS<Params, MGP_t, ARESystem, bd::policy::NNPolicy<PolicyParams>, policy_opt_t, RewardFunction>
{
public:
    BDROPS();
    BDROPS(const Eigen::VectorXd &init_pos, const Eigen::VectorXd &target_position);
    void update(Control::Ptr &ctrl);

    //GETTERS & SETTERS
    MGP_t get_model(){return _model;}
    void set_observation(obs_t obs){_observations = obs;}
    void set_rolloutInfo(bd::RolloutInfo &info){rolloutInfo = info;}
//    void optimize();

private:
    bd::RolloutInfo rolloutInfo;
};
}//are
#endif //BOLEARNER_H
