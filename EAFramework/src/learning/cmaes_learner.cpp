#include "ARE/learning/cmaes_learner.hpp"

using namespace are;

int CMAESLearner::novelty_params::k_value = 15;
double CMAESLearner::novelty_params::archive_adding_prob = 0.4;
double CMAESLearner::novelty_params::novelty_thr = 0.9;


void CMAESLearner::init(double ftarget, std::vector<double> initial_point, double initial_fit){
    int lenStag = settings::getParameter<settings::Integer>(parameters,"#lengthOfStagnation").value;
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#cmaesPopSize").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
    double step_size = settings::getParameter<settings::Double>(parameters,"#CMAESStep").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool elitist_restart = settings::getParameter<settings::Boolean>(parameters,"#elitistRestart").value;
    double novelty_ratio = settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value;
    double novelty_decr = settings::getParameter<settings::Double>(parameters,"#noveltyDecrement").value;
    float pop_stag_thres = settings::getParameter<settings::Float>(parameters,"#populationStagnationThreshold").value;
    float gen_stag_thres = settings::getParameter<settings::Float>(parameters,"#generationalStagnationThreshold").value;


    double lb[_dimension], ub[_dimension];
    for(int i = 0; i < _dimension; i++){
        lb[i] = -max_weight;
        ub[i] = max_weight;
    }

    from_scratch = initial_point.empty();
    if(initial_point.empty())
        initial_point = _rand_num->randVectd(-max_weight,max_weight,_dimension);

    geno_pheno_t gp(lb,ub,_dimension);

    cma::CMAParameters<geno_pheno_t> cmaParam(initial_point,step_size,pop_size,_rand_num->getSeed(),gp);
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);

    _cma_strat = std::make_shared<IPOPCMAStrategy>([](const double*,const int&)->double{return 0;},cmaParam);
    _cma_strat->set_elitist_restart(elitist_restart);
    _cma_strat->set_length_of_stagnation(lenStag);
    _cma_strat->set_novelty_ratio(novelty_ratio);
    _cma_strat->set_novelty_decr(novelty_decr);
    _cma_strat->set_pop_stag_thres(pop_stag_thres);
    _cma_strat->set_gen_stag_thres(gen_stag_thres);
    _cma_strat->set_initial_fitness(initial_fit);
    next_pop();
    initialized = true;

}


void CMAESLearner::update_pop_info(const std::vector<double> &obj, const Eigen::VectorXd &desc, std::vector<std::vector<waypoint>> &trajs){
    if(_population.empty())
        return;
    int idx = _cma_strat->get_population().size();
    IPOPCMAStrategy::individual_t ind;
    ind.genome.resize(_population[idx].rows());
    for(unsigned i = 0; i < _population[idx].rows(); i++)
        ind.genome[i] = _population[idx](i);
    ind.objectives = obj;
    ind.trajectories = trajs;
    ind.descriptor.resize(desc.rows());
    for(int i = 0; i < desc.rows(); i++)
        ind.descriptor[i] = desc(i);
    _cma_strat->add_individual(ind);
    current_nbr_ind--;
}

std::pair<std::vector<double>,std::vector<double>> CMAESLearner::update_ctrl(Control::Ptr &control){

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#nbrHiddenNeurons").value;

    int counter = _cma_strat->get_population().size();

    std::vector<double> weights;
    std::vector<double> bias;
    int i = 0;
    for(; i < _nbr_weights; i++){
        double w = std::tanh(_population[counter](i)); // filtering to put values between -1 and 1
        weights.push_back(w);
    }
    for(; i < _dimension; i++){
        double b = std::tanh(_population[counter](i)); // filtering to put values between -1 and 1
        bias.push_back(b);
    }

    if(nn_type == settings::nnType::FFNN){
        control.reset();
        control = std::make_shared<NN2Control<ffnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(_rand_num);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(_nn_inputs,nb_hidden,_nn_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::ELMAN){
        control.reset();
        control = std::make_shared<NN2Control<elman_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(_rand_num);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(_nn_inputs,nb_hidden,_nn_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::RNN){
        control.reset();
        control = std::make_shared<NN2Control<rnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(_rand_num);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(_nn_inputs,nb_hidden,_nn_outputs,weights,bias);
    }
    else {
        std::cerr << "unknown type of neural network" << std::endl;
    }
    current_nbr_ind++;

    return std::make_pair(weights,bias);

}

void CMAESLearner::iterate(){
    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(novelty_params::k_value >= _population.size())
            novelty_params::k_value = _population.size()/2;
        else novelty_params::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for(const auto& ind : _cma_strat->get_population()){
            Eigen::VectorXd desc(ind.descriptor.size());
            for(size_t i = 0; i < ind.descriptor.size(); i++)
                desc(i) = ind.descriptor[i];
            pop_desc.push_back(desc);
        }
        //compute novelty
        for(auto& ind : _cma_strat->access_population()){
            Eigen::VectorXd ind_desc(ind.descriptor.size());
            for(size_t i = 0; i < ind.descriptor.size(); i++)
                ind_desc(i) = ind.descriptor[i];

            double ind_nov = novelty::sparseness<novelty_params>(Novelty::distances(ind_desc,_novelty_archive,pop_desc));
            ind.objectives.push_back(ind_nov);
        }

        //update archive
        for(const auto& ind : _cma_strat->get_population()){
            Eigen::VectorXd ind_desc(ind.descriptor.size());
            for(size_t i = 0; i < ind.descriptor.size(); i++)
                ind_desc(i) = ind.descriptor[i];
            double ind_nov = ind.objectives.back();
            novelty::update_archive<novelty_params>(ind_desc,ind_nov,_novelty_archive,_rand_num);
        }
    }
    /**/

    _cma_strat->eval();
    _cma_strat->tell();
    _best_solution = _cma_strat->get_best_seen_solution();
    bool stop = _cma_strat->stop();
    double lp_target = settings::getParameter<settings::Double>(parameters,"#learningProgressTarget").value;
    _is_finish = _cma_strat->have_reached_ftarget() || _cma_strat->learning_progress() >= lp_target;
    if(stop){
        bool incrPop = settings::getParameter<settings::Boolean>(parameters,"#incrPop").value;
        bool withRestart = settings::getParameter<settings::Boolean>(parameters,"#withRestart").value;
        bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
        if(withRestart && stop){
            if(verbose)
                std::cout << "Restart !" << std::endl;

            if(incrPop){
                int max_pop_size = settings::getParameter<settings::Integer>(parameters,"#cmaesMaxPopSize").value;
                if(_cma_strat->get_parameters().lambda() < max_pop_size)
                    _cma_strat->lambda_inc();
            }
            _cma_strat->reset_search_state();
        }
    }
}

void CMAESLearner::next_pop(){
    int pop_size = _cma_strat->get_parameters().lambda();
    _archive.emplace(_generation,_cma_strat->get_population());
    _generation++;
    dMat new_samples = _cma_strat->ask();
    _population.clear();
    for(int i = 0; i < pop_size; i++)
        _population.push_back(new_samples.col(i));
    new_population_available = true;
}



bool CMAESLearner::step(){
    _nbr_eval++;
    if(_cma_strat->get_population().size() < _population.size())
        return false;
    if(current_nbr_ind != 0)
        std::cerr << "WARNING current_nbr_ind is not null" << std::endl;

//    if(is_learning_finish())
//        return true;

    iterate();
   // _archive.emplace(_generation,_cma_strat->get_population());

    next_pop();

    return true;
}

bool CMAESLearner::is_learning_finish() const{
    int max_nbr_eval = 0;
    if(from_scratch)
        max_nbr_eval = settings::getParameter<settings::Integer>(parameters,"#cmaesLargeNbrEval").value;
    else max_nbr_eval = settings::getParameter<settings::Integer>(parameters,"#cmaesSmallNbrEval").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
if(verbose)
        std::cout << "INFO - CMAES: Learning ending conditions: " << current_nbr_ind << " = 0 and (nbr evals "
                  << _nbr_eval << " >= " << max_nbr_eval << " or reach target " << _is_finish << " or "
                  << "nbr dropped evals " << nbr_dropped_eval << " > 50)" << std::endl;
    return current_nbr_ind == 0 && (_nbr_eval >= max_nbr_eval || _is_finish || nbr_dropped_eval > 50);

}

std::vector<CMAESLearner::w_b_pair_t> CMAESLearner::get_remaining_population(){
    std::vector<w_b_pair_t>  new_pop;
    for(int k = _population.size() - current_nbr_ind; k < _population.size(); k++){
        auto gen = _population[k];
        std::vector<double> weights;
        std::vector<double> biases;
        int i = 0;
        for(; i < _nbr_weights; i++)
            weights.push_back(std::tanh(gen(i)));
        for(; i < _dimension; i++)
            biases.push_back(std::tanh(gen(i)));

        new_pop.push_back(std::make_pair(weights,biases));
    }
    return new_pop;
}


std::vector<CMAESLearner::w_b_pair_t> CMAESLearner::get_new_population(){
    if(new_population_available && current_nbr_ind == 0){
      std::vector<w_b_pair_t>  new_pop;
	
      for(const auto gen: _population){
        std::vector<double> weights;
        std::vector<double> biases;
        int i = 0;
        for(; i < _nbr_weights; i++)
            weights.push_back(std::tanh(gen(i)));
        for(; i < _dimension; i++)
            biases.push_back(std::tanh(gen(i)));

        new_pop.push_back(std::make_pair(weights,biases));
      }
      current_nbr_ind+=new_pop.size();
      new_population_available = false;
      return new_pop;
    }
    return std::vector<w_b_pair_t>(0);
}



std::string CMAESLearner::archive_to_string() const{
    std::stringstream sstr;
    for(const auto& elt : _archive){
        sstr << elt.first << "\n";
        for(const auto& ind: elt.second){
            for(const double& obj : ind.objectives)
                sstr << obj << ",";
            sstr << "\n";
            for(const double& desc : ind.descriptor)
                sstr << desc << ",";
            sstr << "\n";
            for(const double& gen : ind.genome)
                sstr << gen << ",";
            sstr << "\n";
        }
    }
    return sstr.str();
}

std::string CMAESLearner::to_string() const{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<libcmaes::CMAParameters<geno_pheno_t>>();
    oarch.register_type<libcmaes::RankedCandidate>();
    oarch.register_type<IPOPCMAStrategy>();
    oarch.register_type<CMAESLearner>();
    oarch << *this;
    return sstream.str();
}

void CMAESLearner::from_string(const std::string & str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<libcmaes::CMAParameters<geno_pheno_t>>();
    iarch.register_type<libcmaes::RankedCandidate>();
    iarch.register_type<IPOPCMAStrategy>();
    iarch.register_type<CMAESLearner>();
    iarch >> *this;
    _cma_strat->init_esolver();
}

void CMAESLearner::from_file(const std::string & filename){
    std::ifstream ifs(filename);
    if(!ifs){
        std::cerr << "unable to open : " << filename << std::endl;
        return;
    }
    boost::archive::text_iarchive iarch(ifs);
    iarch.register_type<libcmaes::CMAParameters<geno_pheno_t>>();
    iarch.register_type<libcmaes::RankedCandidate>();
    iarch.register_type<IPOPCMAStrategy>();
    iarch.register_type<CMAESLearner>();
    iarch >> *this;
    ifs.close();
    _cma_strat->init_esolver();
}
