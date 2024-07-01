#include "BayesianOpt.hpp"

using namespace are;
namespace lb = limbo;

BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, max_fun_evals);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, fun_tolerance);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, lbound);
BO_DECLARE_DYN_PARAM(double, Params::opt_cmaes, ubound);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, restarts);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, elitism);
BO_DECLARE_DYN_PARAM(int, Params::opt_cmaes, lambda);
BO_DECLARE_DYN_PARAM(bool, Params::opt_cmaes, handle_uncertainty);
BO_DECLARE_DYN_PARAM(double,Params::acqui_gpucb,delta);

BOLearner::BOLearner()
{
    Params::opt_cmaes::set_lbound(-1.);
    Params::opt_cmaes::set_ubound(1.);

    Params::opt_cmaes::set_max_fun_evals(-1);
    Params::opt_cmaes::set_fun_tolerance(1);
    Params::opt_cmaes::set_restarts(1);
    Params::opt_cmaes::set_elitism(1);
    Params::opt_cmaes::set_lambda(-1);
    Params::opt_cmaes::set_handle_uncertainty(true);
    _current_iteration = 0;
}

BOLearner::BOLearner(const settings::ParametersMapPtr &param){
    double max_weight = settings::getParameter<settings::Float>(param,"#MaxWeight").value;
    Params::opt_cmaes::set_lbound(-max_weight);
    Params::opt_cmaes::set_ubound(max_weight);

    Params::opt_cmaes::set_max_fun_evals(-1);
    Params::opt_cmaes::set_fun_tolerance(1);
    Params::opt_cmaes::set_restarts(1);
    Params::opt_cmaes::set_elitism(1);
    Params::opt_cmaes::set_lambda(-1);
    Params::opt_cmaes::set_handle_uncertainty(true);
    _current_iteration = 0;

    parameters = param;

    double delta = settings::getParameter<settings::Double>(parameters,"#GPUCBParam").value;
    Params::acqui_gpucb::set_delta(delta);

    double _max_dist = settings::getParameter<settings::Double>(parameters,"#maxDistance").value;
  //  _max_dist = sqrt(2*arena_size*arena_size);

   // int obs_type = settings::getParameter<settings::Integer>(parameters,"#observationType").value;


    std::vector<double> targetP = settings::getParameter<settings::Sequence<double>>(parameters,"#targetPosition").value;
    Eigen::VectorXd _target(3);
    _target << targetP[0],targetP[1],targetP[2];

    _reward = DistanceToTarget<Params>(_target,_max_dist);

//    if(obs_type == settings::obsType::FINAL_POS){
//        DistanceToTarget<Params> _reward (_target,_max_dist);
//        _reward = [&](const Eigen::VectorXd& x) -> double {
//            return (1-(x-_target).norm()/_max_dist);
//        };
//    }else if(obs_type == settings::obsType::TRAJECTORY || obs_type == settings::obsType::POS_TRAJ){
//        _reward = [&](const Eigen::VectorXd& x) -> double
//        {
//            int rew = 0;
//            Eigen::VectorXd pos(3);
//            for (int i = 0; i < x.rows(); i+=6) {
//                pos(0) = x(i);
//                pos(1) = x(i+1);
//                pos(2) = x(i+2);
//                rew += (1-(pos-_target).norm()/_max_dist);
//            }
//            return rew;
//        };
//    }
//    else if(obs_type == settings::obsType::OBJ){
//        _reward = [&](const Eigen::VectorXd& x) -> double
//        {
//            return x(0);
//        };
//    }
}

void BOLearner::init_model(int input_size)
{
    _model = model_t(input_size,3);
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

void BOLearner::compute_model(){
    _model.compute(_samples,_observations);
}

void BOLearner::update(const NNParamGenome::Ptr & ctrl_gen)
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    using acqui_optimizer_t =
    typename boost::parameter::binding<args, lb::tag::acquiopt, lb::opt::Cmaes<Params>>::type;

    int nbr_weights = ctrl_gen->get_weights().size();
    int nbr_bias = ctrl_gen->get_biases().size();
    Eigen::VectorXd starting_point = _samples.back();

//    lb::FirstElem aggr;

    std::cout << "start acquisition" << std::endl;

    acqui_optimizer_t acqui_optimizer;

    acquisition_function_t acqui(_model, _current_iteration);

    auto acqui_optimization =
            [&](const Eigen::VectorXd& x, bool g) { return acqui(x, _reward, g); };

    Eigen::VectorXd new_sample = acqui_optimizer(acqui_optimization, starting_point, true);
    std::cout << "finish acquisition" << std::endl;


    std::vector<double> weights, biases;
    int i = 0;
    for(; i < nbr_weights; i++)
        weights.push_back(new_sample(i));
    for(; i < nbr_weights+nbr_bias; i++){
        biases.push_back(new_sample(i));
    }

    ctrl_gen->set_weights(weights);
    ctrl_gen->set_biases(biases);

    if(verbose)
        std::cout << "Delta between previous and current weights : " << (starting_point - new_sample).norm() << std::endl;

    _current_iteration++;
    //        _update_stats(*this, aggr);

}

void BOLearner::update_model()
{
    std::cout << "add samples" << std::endl;
    _model.add_sample(_samples.back(), _observations.back());
    std::cout << "optimize hyperparams" << std::endl;
    _model.optimize_hyperparams();
}
