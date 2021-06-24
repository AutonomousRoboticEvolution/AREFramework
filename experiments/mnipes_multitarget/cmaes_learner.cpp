#include "cmaes_learner.hpp"

using namespace are;

void CMAESLearner::init(std::vector<double> initial_point){
    int lenStag = settings::getParameter<settings::Integer>(parameters,"#lengthOfStagnation").value;

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#cmaesPopSize").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
    double step_size = settings::getParameter<settings::Double>(parameters,"#CMAESStep").value;
    double ftarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool elitist_restart = settings::getParameter<settings::Boolean>(parameters,"#elitistRestart").value;
    double novelty_ratio = settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value;
    double novelty_decr = settings::getParameter<settings::Double>(parameters,"#noveltyDecrement").value;
    float pop_stag_thres = settings::getParameter<settings::Float>(parameters,"#populationStagnationThreshold").value;

    if(initial_point.empty())
        initial_point = _rand_num->randVectd(-max_weight,max_weight,_dimension);

    double lb[_dimension], ub[_dimension];
    for(int i = 0; i < _dimension; i++){
        lb[i] = -max_weight;
        ub[i] = max_weight;
    }

    geno_pheno_t gp(lb,ub,_dimension);

    cma::CMAParameters<geno_pheno_t> cmaParam(initial_point,step_size,pop_size,_rand_num->getSeed(),gp);
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);

    _cma_strat.reset(new IPOPCMAStrategy([](const double*,const int&)->double{},cmaParam));
    _cma_strat->set_elitist_restart(elitist_restart);
    _cma_strat->set_length_of_stagnation(lenStag);
    _cma_strat->set_novelty_ratio(novelty_ratio);
    _cma_strat->set_novelty_decr(novelty_decr);
    _cma_strat->set_pop_stag_thres(pop_stag_thres);
}

void CMAESLearner::update_pop_info(const std::vector<double> &obj, const Eigen::VectorXd &desc){
    IPOPCMAStrategy::individual_t ind;
    for(unsigned i = 0; i < _population[_counter-1].rows(); i++)
        ind.genome.push_back(std::tanh(_population[_counter-1](i)));
    ind.objectives = obj;
    ind.descriptor.resize(desc.rows());
    for(int i = 0; i < desc.rows(); i++)
        ind.descriptor[i] = desc(i);
    _cma_strat->add_individual(ind);
}

void CMAESLearner::iterate(){
    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(Novelty::k_value >= _population.size())
            Novelty::k_value = _population.size()/2;
        else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for(const auto& ind : _cma_strat->get_population()){
            Eigen::VectorXd desc(ind.descriptor.size());
            for(int i = 0; i < ind.descriptor.size(); i++)
                desc(i) = ind.descriptor[i];
            pop_desc.push_back(desc);
        }
        //compute novelty
        for(auto& ind : _cma_strat->access_population()){
            Eigen::VectorXd ind_desc(ind.descriptor.size());
            for(int i = 0; i < ind.descriptor.size(); i++)
                ind_desc(i) = ind.descriptor[i];

            double ind_nov = Novelty::sparseness(Novelty::distances(ind_desc,_novelty_archive,pop_desc));
            ind.objectives.push_back(ind_nov);
        }

        //update archive
        for(const auto& ind : _cma_strat->get_population()){
            Eigen::VectorXd ind_desc(ind.descriptor.size());
            for(int i = 0; i < ind.descriptor.size(); i++)
                ind_desc(i) = ind.descriptor[i];
            double ind_nov = ind.objectives.back();
            Novelty::update_archive(ind_desc,ind_nov,_novelty_archive,_rand_num);
        }
    }
    /**/

    _cma_strat->eval();
    _cma_strat->tell();
    _best_solution = _cma_strat->get_best_seen_solution();
    bool stop = _cma_strat->stop();
    _is_finish = _cma_strat->have_reached_ftarget();
    if(stop){
        bool withRestart = settings::getParameter<settings::Boolean>(parameters,"#withRestart").value;
        bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
        if(withRestart && stop){
            if(verbose)
                std::cout << "Restart !" << std::endl;

            _cma_strat->lambda_inc();
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
    _counter = 0;
    for(int i = 0; i < pop_size; i++)
        _population.push_back(new_samples.col(i));

}

bool CMAESLearner::step(){
    if(_counter < _population.size())
        return false;

    iterate();
    _archive.emplace(_generation,_cma_strat->get_population());

    int max_nbr_eval = settings::getParameter<settings::Integer>(parameters,"#cmaesNbrEval").value;
    if(_nbr_eval >= max_nbr_eval || _is_finish || nbr_dropped_eval > 50)
        return true;

    next_pop();

    return false;
}

std::pair<std::vector<double>,std::vector<double>> CMAESLearner::update_ctrl(Control::Ptr &control){

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;


    std::vector<double> weights;
    std::vector<double> bias;
    int i = 0;
    for(; i < _nbr_weights; i++){
        double w = std::tanh(_population[_counter](i)); // filtering to put values between -1 and 1
        weights.push_back(w);
    }
    for(; i < _dimension; i++){
        double b = std::tanh(_population[_counter](i)); // filtering to put values between -1 and 1
        bias.push_back(b);
    }

    if(nn_type == settings::nnType::FFNN){
        control.reset(new NN2Control<ffnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(_rand_num);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(_nn_inputs,nb_hidden,_nn_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::ELMAN){
        control.reset(new NN2Control<elman_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(_rand_num);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(_nn_inputs,nb_hidden,_nn_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::RNN){
        control.reset(new NN2Control<rnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(_rand_num);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(_nn_inputs,nb_hidden,_nn_outputs,weights,bias);
    }
    else {
        std::cerr << "unknown type of neural network" << std::endl;
    }
    _counter++;
    _nbr_eval++;

    return std::make_pair(weights,bias);

}

std::string CMAESLearner::archive_to_string(){
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
