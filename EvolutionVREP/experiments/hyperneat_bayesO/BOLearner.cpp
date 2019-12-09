#include "BOLearner.h"

using namespace are;
namespace lb = limbo;
BO_DECLARE_DYN_PARAM(int, PolicyParams::nn_policy, hidden_neurons);

BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, max_fun_evals);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, fun_tolerance);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, lbound);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, ubound);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, restarts);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, elitism);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, lambda);
BO_DECLARE_DYN_PARAM(bool, Params::opt_cmaes, handle_uncertainty);




BOLearner::BOLearner() :
    bd::BlackDROPS<Params, MGP_t, ARESystem, bd::policy::NNPolicy<PolicyParams>, policy_opt_t, RewardFunction>()
{
    Params::opt_cmaes::set_lbound(-1.);
    Params::opt_cmaes::set_ubound(1.);

    Params::opt_cmaes::set_max_fun_evals(-1);
    Params::opt_cmaes::set_fun_tolerance(1);
    Params::opt_cmaes::set_restarts(1);
    Params::opt_cmaes::set_elitism(1);
    Params::opt_cmaes::set_lambda(-1);
    Params::opt_cmaes::set_handle_uncertainty(true);

}


BOLearner::BOLearner(const Eigen::VectorXd &init_pos, const Eigen::VectorXd &target_pos) :
    bd::BlackDROPS<Params, MGP_t, ARESystem, bd::policy::NNPolicy<PolicyParams>, policy_opt_t, RewardFunction>()
{
    Params::opt_cmaes::set_lbound(-1.);
    Params::opt_cmaes::set_ubound(1.);

    int max_opt_eval = settings::getParameter<settings::Integer>(parameters,"#maxOptEval").value;
    Params::opt_cmaes::set_max_fun_evals(5);
    Params::opt_cmaes::set_fun_tolerance(5);
    Params::opt_cmaes::set_restarts(1);
    Params::opt_cmaes::set_elitism(0);
    Params::opt_cmaes::set_lambda(-1);
    Params::blackdrops::set_verbose(true);
    Params::blackdrops::set_stochastic(true);

}

//void BOLearner::optimize()
//{
//    // initial step-size, i.e. estimated initial parameter error.
//    double sigma = 0.5;
//    std::vector<double> x0(init.data(), init.data() + init.size());

//    libcmaes::CMAParameters<> cmaparams(x0, sigma, Params::opt_cmaes::lambda());
//    _set_common_params(cmaparams, dim);

//    auto pfunc = CMAStrategy<CovarianceUpdate, GenoPheno<NoBoundStrategy>>::_defaultPFunc;

//    // the optimization itself
//    libcmaes::CMASolutions cmasols = cmaes<>(f_cmaes, cmaparams, pfunc);
//    if (Params::opt_cmaes::stochastic() || Params::opt_cmaes::handle_uncertainty())
//        return cmasols.xmean();

//    return cmasols.get_best_seen_candidate().get_x_dvec();
//}

void BOLearner::update(const Control::Ptr & ctrl)
{
    NEAT::NeuralNetwork nn = std::dynamic_pointer_cast<NNControl>(ctrl)->nn;

    int nbr_hidden = nn.m_neurons.size() - (nn.m_num_inputs + nn.m_num_outputs);

    PolicyParams::nn_policy::set_hidden_neurons(nbr_hidden);

    auto func = [&](Eigen::VectorXd x) -> Eigen::VectorXd
    {
        std::vector<double> input(x.rows());
        for(int i = 0; i < input.size(); i++)
            input[i] = x(i);
        std::vector<double> output = ctrl->update(input);
        Eigen::VectorXd y(output.size());
        for(int i = 0; i < output.size(); i++)
            y(i) = output[i];
    };




}
