#include "BlackDROPS.hpp"

using namespace are;
namespace lb = limbo;
namespace bd = blackdrops;
BO_DECLARE_DYN_PARAM(int, PolicyParams::nn_policy, hidden_neurons);
BO_DECLARE_DYN_PARAM(bool, Params::blackdrops, verbose);
BO_DECLARE_DYN_PARAM(bool, Params::blackdrops, stochastic);
BO_DECLARE_DYN_PARAM(double, Params::blackdrops, boundary);

BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, max_fun_evals);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, fun_tolerance);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, lbound);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, ubound);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, restarts);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, elitism);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, lambda);
BO_DECLARE_DYN_PARAM(bool, Params::opt_cmaes, handle_uncertainty);




BlackDROPS::BlackDROPS() :
    bd::BlackDROPS<Params, MGP_t, ARESystem, bd::policy::NNPolicy<PolicyParams>, policy_opt_t, RewardFunction>()
{
    Params::blackdrops::set_boundary(1.);
    Params::opt_cmaes::set_lbound(-1.);
    Params::opt_cmaes::set_ubound(1.);

    Params::opt_cmaes::set_max_fun_evals(-1);
    Params::opt_cmaes::set_fun_tolerance(1);
    Params::opt_cmaes::set_restarts(1);
    Params::opt_cmaes::set_elitism(1);
    Params::opt_cmaes::set_lambda(-1);
    Params::opt_cmaes::set_handle_uncertainty(true);
    Params::blackdrops::set_verbose(true);
    Params::blackdrops::set_stochastic(true);
}


BlackDROPS::BlackDROPS(const Eigen::VectorXd &init_pos, const Eigen::VectorXd &target_pos) :
    bd::BlackDROPS<Params, MGP_t, ARESystem, bd::policy::NNPolicy<PolicyParams>, policy_opt_t, RewardFunction>()
{
    Params::blackdrops::set_boundary(1.);
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

void BDROPS::update(Control::Ptr & ctrl)
{
    NEAT::NeuralNetwork nn = std::dynamic_pointer_cast<NNControl>(ctrl)->nn;


    int nbr_hidden = nn.m_neurons.size() - (nn.m_num_inputs + nn.m_num_outputs);

    PolicyParams::nn_policy::set_hidden_neurons(nbr_hidden);

    _policy = bd::policy::NNPolicy<PolicyParams>();

    Eigen::VectorXd nn_params(nn.m_connections.size());
    for(int i = 0; i < nn.m_connections.size(); i++)
        nn_params(i) = nn.m_connections[i].m_weight;
    for(int i = 0; i < nn.m_neurons.size(); i++)
        nn_params(i) = nn.m_neurons[i].m_bias;

    _policy.set_params(nn_params);

    _boundary = Params::blackdrops::boundary();
    _random_policies = false;

    std::cout << "LEARN MODEL" << std::endl;
    learn_model();
    std::cout << "Optimize Policy" << std::endl;
    nn_params = _policy_optimizer(
                std::bind(&BDROPS::_optimize_policy, this, std::placeholders::_1, std::placeholders::_2),
                nn_params, true);
    _policy.set_params(nn_params);
//    std::vector<double> R;
//    _robot.execute_dummy(_policy,_model,_reward,Params::blackdrops::T(),R);

//    optimize_policy(0); ///@todo find how to set properly this argument

    for(int i = 0; i < nn.m_connections.size(); i++)
        nn.m_connections[i].m_weight = _policy.params()[i];

    std::dynamic_pointer_cast<NNControl>(ctrl)->nn = nn;
}
