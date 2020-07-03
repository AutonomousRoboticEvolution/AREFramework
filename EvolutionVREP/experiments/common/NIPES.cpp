#include "NIPES.hpp"


using namespace are;

std::map<int,std::string> IPOPCMAStrategy::scriterias = {{cma::CONT,"OK"},
                                                           {cma::AUTOMAXITER,"The automatically set maximal number of iterations per run has been reached"},
                                                           {cma::TOLHISTFUN,"[Success] The optimization has converged"},
                                                           {cma::EQUALFUNVALS,"[Partial Success] The objective function values are the same over too many iterations, check the formulation of your objective function"},
                                                           {cma::TOLX,"[Partial Success] All components of covariance matrix are very small (e.g. < 1e-12)"},
                                                           {cma::TOLUPSIGMA,"[Error] Mismatch between step size increase and decrease of all eigenvalues in covariance matrix. Try to restart the optimization."},
                                                           {cma::STAGNATION,"[Partial Success] Median of newest values is not smaller than the median of older values"},
                                                           {cma::CONDITIONCOV,"[Error] The covariance matrix's condition numfber exceeds 1e14. Check out the formulation of your problem"},
                                                           {cma::NOEFFECTAXIS,"[Partial Success] Mean remains constant along search axes"},
                                                           {cma::NOEFFECTCOOR,"[Partial Success] Mean remains constant in coordinates"},
                                                           {cma::MAXFEVALS,"The maximum number of function evaluations allowed for optimization has been reached"},
                                                           {cma::MAXITER,"The maximum number of iterations specified for optimization has been reached"},
                                                           {cma::FTARGET,"[Success] The objective function target value has been reached"}};

bool IPOPCMAStrategy::reach_ftarget(){
    cma::LOG_IF(cma::INFO,!_parameters.quiet()) << "Best fitness : "  << best_fitnesses.back() << std::endl;

    if (_parameters.get_ftarget() != std::numeric_limits<double>::infinity())
    {
        if (best_fitnesses.back() <= _parameters.get_ftarget())
        {
            std::stringstream sstr;
            sstr << "stopping criteria fTarget => fvalue=" << best_fitnesses.back() << " / ftarget=" << _parameters.get_ftarget();
            log_stopping_criterias.push_back(sstr.str());
            cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
            return true;
        }
    }
    return false;
}

bool IPOPCMAStrategy::pop_desc_stagnation(){
   std::vector<Eigen::VectorXd> descriptors;
   for (const auto& ind: _pop)
       descriptors.push_back(std::dynamic_pointer_cast<NN2Individual>(ind)->descriptor());

   Eigen::VectorXd mean = Eigen::VectorXd::Zero(3);
   for(Eigen::VectorXd desc : descriptors){
       mean += desc;
   }
   mean = mean/static_cast<double>(descriptors.size());

   Eigen::VectorXd stddev = Eigen::VectorXd::Zero(3);
   for(Eigen::VectorXd desc : descriptors)
       stddev += (desc - mean).cwiseProduct(desc - mean);

   bool stop = true;
   for(int i = 0; i < stddev.rows(); i++)
       stop = stop && sqrt(stddev(i/static_cast<double>(descriptors.size() - 1))) <= pop_stag_thres;

   if(stop){
       std::stringstream sstr;
       sstr << "Stopping : standard deviation of the descriptor population is smaller than " << pop_stag_thres << " : " << stddev;
       log_stopping_criterias.push_back(sstr.str());
       cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
   }
   return stop;
}

bool IPOPCMAStrategy::pop_fit_stagnation(){
    std::vector<double> fvalues;
    for(const auto& ind : _pop)
       fvalues.push_back(ind->getObjectives()[0]);



    double mean=0.0;
    for(double fv : fvalues)
        mean+=fv;
    mean = mean/static_cast<double>(fvalues.size());

    double stddev = 0.0;
    for(double fv : fvalues)
        stddev+= (fv - mean)*(fv-mean);

    stddev = sqrt(stddev/static_cast<double>(fvalues.size()-1));
    cma::LOG_IF(cma::INFO,!_parameters.quiet()) << "pop standard deviation : " << stddev << std::endl;

    if(stddev <= pop_stag_thres){
        std::stringstream sstr;
        sstr << "Stopping : standard deviation of the population is smaller than 0.05 : " << stddev;
        log_stopping_criterias.push_back(sstr.str());
        cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
        return true;
    }else return false;
}

bool IPOPCMAStrategy::best_sol_stagnation(){
    if(best_fitnesses.size() < len_of_stag)
        return false;
    double mean = 0.0;
    for(size_t i = best_fitnesses.size() - len_of_stag
        ; i < best_fitnesses.size(); i++){
        mean += best_fitnesses[i];
    }
    mean = mean/static_cast<float>(len_of_stag);
    double stddev = 0.0;
    for(size_t i = best_fitnesses.size() - len_of_stag
        ; i < best_fitnesses.size(); i++){
        stddev += (best_fitnesses[i] - mean)*(best_fitnesses[i] - mean);
    }
    stddev = sqrt(stddev/static_cast<float>(len_of_stag-1));

    if(stddev <= 0.05){
        std::stringstream sstr;
        sstr << "Stopping : standard deviation of the last " << len_of_stag <<  " best fitnesses is smaller than 0.05 : " << stddev;
        log_stopping_criterias.push_back(sstr.str());
        cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
        return true;
    }else return false;
}

void IPOPCMAStrategy::eval(const dMat &candidates, const dMat &phenocandidates){
    // custom eval.
    _solutions.candidates().clear();
    for (int r=0;r<_pop.size();r++)
    {
        std::vector<double> genome = std::dynamic_pointer_cast<NNParamGenome>(_pop[r]->get_ctrl_genome())->get_full_genome();
        dVec x(genome.size());
        for(int i = 0; i < genome.size(); i++)
            x(i) = genome[i];

        double reward = 0;
        int i = 0;
        for(; i < _pop[r]->getObjectives().size() - 1; i++)
            reward += 1 - _pop[r]->getObjectives()[i];

        reward = reward/static_cast<double>(_pop[r]->getObjectives().size() - 1);

        double fvalue = (1-novelty_ratio)*reward
                + novelty_ratio*(1-_pop[r]->getObjectives()[i]);

        _solutions.candidates().push_back(cma::Candidate(fvalue,x));
    }
    update_fevals(candidates.cols());
}

void IPOPCMAStrategy::tell()
{
    ipop_cmaes_t::tell();
    std::vector<double> best_sample;
    best_fitnesses.push_back(best_fitness(best_sample));
    if(novelty_ratio > 0)
        novelty_ratio -= novelty_decr;
    if(best_fitnesses.back() < best_seen_solution.first || best_fitnesses.size() == 1)
        best_seen_solution = std::make_pair(best_fitnesses.back(),best_sample);
    inc_iter();
}

bool IPOPCMAStrategy::stop()
{
    reached_ft = reach_ftarget();
    bool ipop_stop = ipop_cmaes_t::stop();
    bool pop_stag = pop_desc_stagnation();
    bool best_sol_stag = false;
    if(len_of_stag > 0)
        best_sol_stag = best_sol_stagnation();

    if(ipop_stop){
        log_stopping_criterias.push_back(scriterias[_solutions.run_status()]);
    }
    return  reached_ft || pop_stag || best_sol_stag  || ipop_stop;
}

void IPOPCMAStrategy::reset_search_state()
{
    if(elitist_restart)
        _parameters.set_x0(best_seen_solution.second,best_seen_solution.second);

    ipop_cmaes_t::reset_search_state();
    novelty_ratio = start_novelty_ratio;
    best_fitnesses.clear();
}

double IPOPCMAStrategy::best_fitness(std::vector<double> &best_sample){
    double bf = 1.;
    for(const auto& ind : _pop){
        if(bf > 1 - ind->getObjectives()[0]){
            bf = 1 - ind->getObjectives()[0];
            best_sample = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();
        }
    }
    return bf;
}

void NIPES::init(){
    int lenStag = settings::getParameter<settings::Integer>(parameters,"#lengthOfStagnation").value;

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
    double step_size = settings::getParameter<settings::Double>(parameters,"#CMAESStep").value;
    double ftarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool elitist_restart = settings::getParameter<settings::Boolean>(parameters,"#elitistRestart").value;
    double novelty_ratio = settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value;
    double novelty_decr = settings::getParameter<settings::Double>(parameters,"#noveltyDecrement").value;
    float pop_stag_thres = settings::getParameter<settings::Float>(parameters,"#populationStagnationThreshold").value;

    Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProb").value;

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }


    std::vector<double> initial_point = randomNum->randVectd(-max_weight,max_weight,nbr_weights + nbr_bias);


    double lb[nbr_weights+nbr_bias], ub[nbr_weights+nbr_bias];
    for(int i = 0; i < nbr_weights+nbr_bias; i++){
        lb[i] = -max_weight;
        ub[i] = max_weight;
    }

    geno_pheno_t gp(lb,ub,nbr_weights+nbr_bias);

    cma::CMAParameters<geno_pheno_t> cmaParam(initial_point,step_size,pop_size,randomNum->getSeed(),gp);
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);


    cmaStrategy.reset(new IPOPCMAStrategy([](const double*,const int&)->double{},cmaParam));
    cmaStrategy->set_elitist_restart(elitist_restart);
    cmaStrategy->set_length_of_stagnation(lenStag);
    cmaStrategy->set_novelty_ratio(novelty_ratio);
    cmaStrategy->set_novelty_decr(novelty_decr);
    cmaStrategy->set_pop_stag_thres(pop_stag_thres);

    dMat init_samples = cmaStrategy->ask();

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);

    for(int u = 0; u < pop_size; u++){

        for(int v = 0; v < nbr_weights; v++)
            weights[v] = init_samples(v,u);
        for(int w = nbr_weights; w < nbr_weights+nbr_bias; w++)
            biases[w-nbr_weights] = init_samples(w,u);

        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        ctrl_gen->set_weights(weights);
        ctrl_gen->set_biases(biases);
        Individual::Ptr ind(new NN2Individual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }

}

void NIPES::epoch(){

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool withRestart = settings::getParameter<settings::Boolean>(parameters,"#withRestart").value;
    bool incrPop = settings::getParameter<settings::Boolean>(parameters,"#incrPop").value;
    bool elitist_restart = settings::getParameter<settings::Boolean>(parameters,"#elitistRestart").value;
    double energy_budget = settings::getParameter<settings::Double>(parameters,"#energyBudget").value;
    bool energy_reduction = settings::getParameter<settings::Boolean>(parameters,"#energyReduction").value;

    /**Energy Cost**/
    if(energy_reduction){
        for(const auto &ind : population){
            double ec = std::dynamic_pointer_cast<NN2Individual>(ind)->get_energy_cost();
            if(ec > energy_budget) ec = energy_budget;
            std::dynamic_pointer_cast<NN2Individual>(ind)->addObjective(1 - ec/energy_budget);
        }
    }

    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(Novelty::k_value >= population.size())
            Novelty::k_value = population.size()/2;
        else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for(const auto& ind : population)
            pop_desc.push_back(std::dynamic_pointer_cast<NN2Individual>(ind)->descriptor());
        //compute novelty
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<NN2Individual>(ind)->descriptor();
            double ind_nov = Novelty::sparseness(Novelty::distances(ind_desc,archive,pop_desc));
            std::dynamic_pointer_cast<NN2Individual>(ind)->addObjective(ind_nov);
        }

        //update archive
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<NN2Individual>(ind)->descriptor();
            double ind_nov = ind->getObjectives().back();
            Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
        }
    }
    /**/

    cmaStrategy->set_population(population);
    cmaStrategy->eval();
    cmaStrategy->tell();
    bool stop = cmaStrategy->stop();
    if(cmaStrategy->have_reached_ftarget()){
        _is_finish = true;
        return;
    }

    if(withRestart && stop){
        if(verbose)
            std::cout << "Restart !" << std::endl;

        cmaStrategy->capture_best_solution(best_run);

        if(incrPop)
            cmaStrategy->lambda_inc();

        cmaStrategy->reset_search_state();
        if(!elitist_restart){
            float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
            cmaStrategy->get_parameters().set_x0(-max_weight,max_weight);
        }
    }
}

void NIPES::init_next_pop(){
    int pop_size = cmaStrategy->get_parameters().lambda();

    dMat new_samples = cmaStrategy->ask();

    int nbr_weights = std::dynamic_pointer_cast<NNParamGenome>(population[0]->get_ctrl_genome())->get_weights().size();
    int nbr_bias = std::dynamic_pointer_cast<NNParamGenome>(population[0]->get_ctrl_genome())->get_biases().size();

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);

    population.clear();
    for(int i = 0; i < pop_size ; i++){

        for(int j = 0; j < nbr_weights; j++)
            weights[j] = new_samples(j,i);
        for(int j = nbr_weights; j < nbr_weights+nbr_bias; j++)
            biases[j-nbr_weights] = new_samples(j,i);

        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        ctrl_gen->set_weights(weights);
        ctrl_gen->set_biases(biases);
        Individual::Ptr ind(new NN2Individual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void NIPES::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    population[indIdx]->setObjectives(objectives);
}


bool NIPES::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;
    reevaluated++;

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_trajectory(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_trajectory());
    }

//    int nbReEval = settings::getParameter<settings::Integer>(parameters,"#numberOfReEvaluation").value;
//    if(reevaluated < nbReEval)
//        return false;

//    reevaluated = 0;
    return true;
}

bool NIPES::is_finish(){
    int maxNbrEval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;

    return /*_is_finish ||*/ numberEvaluation >= maxNbrEval;
}

bool NIPES::finish_eval(){

    float tPos[3];
    tPos[0] = settings::getParameter<settings::Double>(parameters,"#target_x").value;
    tPos[1] = settings::getParameter<settings::Double>(parameters,"#target_y").value;
    tPos[2] = settings::getParameter<settings::Double>(parameters,"#target_z").value;
    double fTarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    double arenaSize = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;

    auto distance = [](float* a,float* b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };

    int handle = population[currentIndIndex]->get_morphology()->getMainHandle();
    float pos[3];
    simGetObjectPosition(handle,-1,pos);
    double dist = 1 - distance(pos,tPos)/sqrt(2*arenaSize*arenaSize);

    return  dist < fTarget;
}
