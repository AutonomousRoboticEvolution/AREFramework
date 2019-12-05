#include "BOLearner.h"

using namespace are;

BOLearner::BOLearner(const Eigen::VectorXd &init_pos, const Eigen::VectorXd &target_pos)
{
//    Params::blackdrops::set_boundary(cmd_arguments.boundary()); ?
//    Params::opt_cmaes::set_lbound(-cmd_arguments.boundary()); ?
//    Params::opt_cmaes::set_ubound(cmd_arguments.boundary()); ?

    int max_opt_eval = settings::getParameter<settings::Integer>(parameters,"#maxOptEval").value;
    Params::opt_cmaes::set_max_fun_evals(5);
    Params::opt_cmaes::set_fun_tolerance(5);
    Params::opt_cmaes::set_restarts(1);
    Params::opt_cmaes::set_elitism(0);
    Params::opt_cmaes::set_lambda(-1);

}

void BOLearner::update(const Control::Ptr & ctrl)
{
    NEAT::NeuralNetwork nn = std::dynamic_pointer_cast<NNControl>(ctrl)->nn;
    int nbr_hidden = nn.m_neurons.size() - (nn.m_num_inputs + nn.m_num_outputs);
    PolicyParams::nn_policy::set_hidden_neurons(nbr_hidden);
    Eigen::VectorXd nn_params(nn.m_connections.size());
    for(int i = 0; i < nn.m_connections.size(); i++)
        nn_params(i) = nn.m_connections[i].m_weight;

    _policy.set_params(nn_params);

    learn_model();
    optimize_policy(0); ///@todo find how to set properly this argument

    for(int i = 0; i < nn.m_connections.size(); i++)
        nn.m_connections[i].m_weight = _policy.params()[i];

    std::dynamic_pointer_cast<NNControl>(ctrl)->nn = nn;
}
