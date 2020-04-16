#include "BOCMAES.hpp"


using namespace are;

std::map<int,std::string> customCMAStrategy::scriterias = {{cmaes::CONT,"OK"},
                                                           {cmaes::AUTOMAXITER,"The automatically set maximal number of iterations per run has been reached"},
                                                           {cmaes::TOLHISTFUN,"[Success] The optimization has converged"},
                                                           {cmaes::EQUALFUNVALS,"[Partial Success] The objective function values are the same over too many iterations, check the formulation of your objective function"},
                                                           {cmaes::TOLX,"[Partial Success] All components of covariance matrix are very small (e.g. < 1e-12)"},
                                                           {cmaes::TOLUPSIGMA,"[Error] Mismatch between step size increase and decrease of all eigenvalues in covariance matrix. Try to restart the optimization."},
                                                           {cmaes::STAGNATION,"[Partial Success] Median of newest values is not smaller than the median of older values"},
                                                           {cmaes::CONDITIONCOV,"[Error] The covariance matrix's condition numfber exceeds 1e14. Check out the formulation of your problem"},
                                                           {cmaes::NOEFFECTAXIS,"[Partial Success] Mean remains constant along search axes"},
                                                           {cmaes::NOEFFECTCOOR,"[Partial Success] Mean remains constant in coordinates"},
                                                           {cmaes::MAXFEVALS,"The maximum number of function evaluations allowed for optimization has been reached"},
                                                           {cmaes::MAXITER,"The maximum number of iterations specified for optimization has been reached"},
                                                           {cmaes::FTARGET,"[Success] The objective function target value has been reached"}};

bool customCMAStrategy::reach_ftarget(){
    cmaes::LOG_IF(cmaes::INFO,!_parameters.quiet()) << "Best fitness : "  << best_fitnesses.back() << std::endl;

    if (_parameters.get_ftarget() != std::numeric_limits<double>::infinity())
    {
        if (best_fitnesses.back() <= _parameters.get_ftarget())
        {
            std::stringstream sstr;
            sstr << "stopping criteria fTarget => fvalue=" << best_fitnesses.back() << " / ftarget=" << _parameters.get_ftarget();
            log_stopping_criterias.push_back(sstr.str());
            cmaes::LOG_IF(cmaes::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
            return true;
        }
    }
    return false;
}

bool customCMAStrategy::pop_stagnation(){
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
    cmaes::LOG_IF(cmaes::INFO,!_parameters.quiet()) << "pop standard deviation : " << stddev << std::endl;

    if(stddev <= 0.05){
        std::stringstream sstr;
        sstr << "Stopping : standard deviation of the population is smaller than 0.05 : " << stddev;
        log_stopping_criterias.push_back(sstr.str());
        cmaes::LOG_IF(cmaes::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
        return true;
    }else return false;
}

bool customCMAStrategy::best_sol_stagnation(){
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
        cmaes::LOG_IF(cmaes::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
        best_fitnesses.clear();
        return true;
    }else return false;
}

void customCMAStrategy::eval(const dMat &candidates, const dMat &phenocandidates){
    // custom eval.
    for (int r=0;r<_pop.size();r++)
    {
        std::vector<double> genome = std::dynamic_pointer_cast<NNParamGenome>(_pop[r]->get_ctrl_genome())->get_full_genome();
        dVec x(genome.size());
        for(int i = 0; i < genome.size(); i++)
            x(i) = genome[i];

        _solutions.get_candidate(r).set_x(x);
        double fvalue = (1-novelty_ratio)*(1-_pop[r]->getObjectives()[0])
                + novelty_ratio*(1-_pop[r]->getObjectives()[1]);
        _solutions.get_candidate(r).set_fvalue(fvalue);

    }
    update_fevals(candidates.cols());
}

void customCMAStrategy::tell()
{
    ipop_cmaes_t::tell();
    best_fitnesses.push_back(best_fitness());
    if(novelty_ratio > 0)
        novelty_ratio -= novelty_decr;
}

bool customCMAStrategy::stop()
{
    reached_ft = reach_ftarget();
    bool ipop_stop = ipop_cmaes_t::stop();
    bool pop_stag = pop_stagnation();
    bool best_sol_stag = false;
    if(len_of_stag > 0)
        best_sol_stag = best_sol_stagnation();

    if(ipop_stop){
        log_stopping_criterias.push_back(scriterias[_solutions.run_status()]);
    }
    return  reached_ft || pop_stag || best_sol_stag  || ipop_stop;
}

void customCMAStrategy::reset_search_state()
{
    ipop_cmaes_t::reset_search_state();
    if(elitist_restart){
        _parameters.set_x0(_solutions.get_best_seen_candidate().get_x_dvec_ref());
    }
    novelty_ratio = start_novelty_ratio;
}

double customCMAStrategy::best_fitness(){
    double bf = 1.;
    for(const auto& ind : _pop){
        if(bf > 1 - ind->getObjectives()[0])
            bf = 1 - ind->getObjectives()[0];
    }
    return bf;
}

void BOCMAES::init(){

    int init_bo_dataset = settings::getParameter<settings::Integer>(parameters,"#initBODataSet").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;

    Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProb").value;

    NNGenome nn_gen(randomNum,parameters);
    NEAT::NeuralNetwork nn;
    nn_gen.init();
    nn_gen.buildPhenotype(nn);
    int nbr_weights = nn.m_connections.size();
    int nbr_biases = nn.m_neurons.size();

    Eigen::MatrixXd init_samples = limbo::tools::random_lhs(nbr_weights + nbr_biases,init_bo_dataset);

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_biases);

    for(int u = 0; u < init_bo_dataset; u++){

        for(int v = 0; v < nbr_weights; v++)
            weights[v] = max_weight*init_samples(u,v);
        for(int w = nbr_weights; w < nbr_weights+nbr_biases; w++)
            biases[w-nbr_weights] = max_weight*init_samples(u,w);

        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        ctrl_gen->set_weights(weights);
        ctrl_gen->set_biases(biases);
        BOLearner::Ptr learner(new BOLearner(parameters));
        Individual::Ptr ind(new BOCMAESIndividual(morph_gen,ctrl_gen,learner));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }

}

void BOCMAES::cmaes_init_pop()
{
    int lenStag = settings::getParameter<settings::Integer>(parameters,"#lengthOfStagnation").value;

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    //Transfer Knowledge from BO to CMAES by taking the pop_size best solution produced by BO
    BOLearner learner(parameters);
    Eigen::VectorXd target;
    target << settings::getParameter<settings::Double>(parameters,"#target_x").value,
            settings::getParameter<settings::Double>(parameters,"#target_y").value,
            settings::getParameter<settings::Double>(parameters,"#target_z").value;
    learner.set_target(target);
    std::vector<size_t> idx(observations.size());
    std::iota(idx.begin(),idx.end(),0);

    std::sort(idx.begin(),idx.end(),
              [&](const size_t &i1,const size_t &i2) -> bool{return learner.reward(observations[i1]) > learner.reward(observations[i2]);});
    population.clear();

    NNGenome nn_gen(randomNum,parameters);
    NEAT::NeuralNetwork nn;
    nn_gen.init();
    nn_gen.buildPhenotype(nn);
    int nbr_weights = nn.m_connections.size();
    int nbr_bias = nn.m_neurons.size();
    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);

    Eigen::VectorXd mean_sample = Eigen::VectorXd::Zero(samples[0].rows());

    for (int i = 0;i < pop_size; i++) {

        mean_sample += samples[idx[i]];

        for(int v = 0; v < weights.size(); v++)
            weights[v] = samples[idx[i]](v);
        for(int w = weights.size(); w < weights.size()+biases.size(); w++)
            biases[w-weights.size()] = samples[idx[i]](w);

        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        ctrl_gen->set_weights(weights);
        ctrl_gen->set_biases(biases);
        BOLearner::Ptr learner(new BOLearner(parameters));
        learner->set_target(target);
        Individual::Ptr ind(new BOCMAESIndividual(morph_gen,ctrl_gen,learner));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        ind->setObjectives({learner->reward(observations[idx[i]])});
        std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->
                set_final_position({observations[idx[i]](0),observations[idx[i]](1),observations[idx[i]](2)});
        population.push_back(ind);
    }


    mean_sample /= static_cast<double>(pop_size);
    std::vector<double> initial_point;
    for(int i = 0; i < mean_sample.rows(); i++)
        initial_point.push_back(mean_sample(i));

    //Init CMAES
    double step_size = settings::getParameter<settings::Double>(parameters,"#CMAESStep").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
    double ftarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool elitist_restart = settings::getParameter<settings::Boolean>(parameters,"#elitistRestart").value;
    double novelty_ratio = settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value;
    double novelty_decr = settings::getParameter<settings::Double>(parameters,"#noveltyDecrement").value;

    double lb[nbr_weights+nbr_bias], ub[nbr_weights+nbr_bias];
    for(int i = 0; i < nbr_weights+nbr_bias; i++){
        lb[i] = -max_weight;
        ub[i] = max_weight;
    }
    geno_pheno_t gp(lb,ub,nbr_weights+nbr_bias);


    cmaes::CMAParameters<geno_pheno_t> cmaParam(initial_point,step_size,pop_size,randomNum->getSeed(),gp);
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);

    cmaStrategy.reset(new customCMAStrategy([](const double*,const int&)->double{},cmaParam));
    cmaStrategy->set_elitist_restart(elitist_restart);
    cmaStrategy->set_length_of_stagnation(lenStag);
    cmaStrategy->set_novelty_ratio(novelty_ratio);
    cmaStrategy->set_novelty_decr(novelty_decr);

    cmaStrategy->ask();
}

void BOCMAES::epoch(){
    if(boIsActive){
        if(generation == 0){
            int rnd_i = randomNum->randInt(0,population.size()-1);
            BOCMAESIndividual::Ptr ind = population[rnd_i];
            population.clear();
            population.push_back(ind);
            currentIndIndex = 0;
        }
        bo_epoch();
    }
    else{
        int nbr_bo_iter = settings::getParameter<settings::Integer>(parameters,"#numberBOIteration").value;
        int bo_init_dataset = settings::getParameter<settings::Integer>(parameters,"#initBODataSet").value;

        if(numberEvaluation == nbr_bo_iter + bo_init_dataset)
        {
            cmaes_init_pop();
        }
        cmaes_epoch();
    }
}

void BOCMAES::bo_epoch(){
    Individual::Ptr ind = population[currentIndIndex];


    if(currentFitnesses.size() == 1){
        std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->compute_model(observations,samples);
    }

    Eigen::VectorXd target(3);
    target << settings::getParameter<settings::Double>(parameters,"#target_x").value,
            settings::getParameter<settings::Double>(parameters,"#target_y").value,
            settings::getParameter<settings::Double>(parameters,"#target_z").value;

    std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->update_learner(observations, samples,target);

    ind.reset();
}

void BOCMAES::cmaes_epoch(){

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool withRestart = settings::getParameter<settings::Boolean>(parameters,"#withRestart").value;
    bool incrPop = settings::getParameter<settings::Boolean>(parameters,"#incrPop").value;


    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(Novelty::k_value >= population.size())
            Novelty::k_value = population.size()/2;
        else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for(const auto& ind : population)
            pop_desc.push_back(std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->descriptor());
        //compute novelty
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->descriptor();
            double ind_nov = Novelty::sparseness(Novelty::distances(ind_desc,archive,pop_desc));
            std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->addObjective(ind_nov);
        }

        //update archive
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->descriptor();
            double ind_nov = ind->getObjectives().back();
            Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
        }
    }
    /**/

    cmaStrategy->set_population(population);
    cmaStrategy->eval();
    cmaStrategy->tell();
    if(withRestart && cmaStrategy->stop()){
        if(verbose)
            std::cout << "Restart !" << std::endl;

        cmaStrategy->capture_best_solution(best_run);
        if(cmaStrategy->have_reached_ftarget()){
            _is_finish = true;
            return;
        }

        if(incrPop)
            cmaStrategy->lambda_inc();

        cmaStrategy->reset_search_state();

    }
}

void BOCMAES::init_next_pop(){
    if(!boIsActive)
        cmaes_next_pop();
}



void BOCMAES::cmaes_next_pop(){
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
        BOLearner::Ptr learner(new BOLearner);
        learner->set_parameters(parameters);
        Individual::Ptr ind(new BOCMAESIndividual(morph_gen,ctrl_gen,learner));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
    cmaStrategy->inc_iter();

}

void BOCMAES::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool BOCMAES::update(const Environment::Ptr & env){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int bo_init_dataset = settings::getParameter<settings::Integer>(parameters,"#initBODataSet").value;
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    if(simulator_side)
        std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());


    if(instance_type == settings::INSTANCE_REGULAR || !simulator_side){
        int nbr_bo_iter = settings::getParameter<settings::Integer>(parameters,"#numberBOIteration").value;

        boIsActive = numberEvaluation - bo_init_dataset < nbr_bo_iter;


        if(boIsActive){
            Individual::Ptr ind = population[currentIndIndex];

            //add last fitness and NN weights in the database for the Bayesian optimizer
            Eigen::VectorXd o(3);
            std::vector<double> final_pos = std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->get_final_position();
            o(0) = final_pos[0];
            o(1) = final_pos[1];
            o(2) = final_pos[2];

            observations.push_back(o);

            std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_weights();
            std::vector<double> biases = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_biases();
            Eigen::VectorXd s(weights.size() + biases.size());
            int i = 0;
            for(const auto &w : weights){
                s(i) = w;
                i++;
            }
            for(const auto &b : biases){
                s(i) = b;
                i++;
            }

            samples.push_back(s);

            if(verbose)
                std::cout << "Size of dataset for BO : " <<  observations.size() << std::endl;
        }
    }


    return true;
}



bool BOCMAES::is_finish(){
    int maxNbrEval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    return _is_finish || numberEvaluation >= maxNbrEval;
}
