#include "CMAES.hpp"


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

bool customCMAStrategy::pop_stagnation(){
    std::vector<double> fvalues;
    for(int i = 0; i < _parameters.lambda(); i++)
       fvalues.push_back(_solutions.get_candidate(i).get_fvalue());

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
        return true;
    }else return false;
}

void CMAES::init(){
    int lenStag = settings::getParameter<settings::Integer>(parameters,"#lengthOfStagnation").value;

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    double step_size = settings::getParameter<settings::Double>(parameters,"#CMAESStep").value;
    double ftarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool elitist_restart = settings::getParameter<settings::Boolean>(parameters,"#elitistRestart").value;

    std::vector<double> initial_point;

    NNGenome::Ptr nn_gen(new NNGenome(randomNum,parameters));
    NEAT::RNG rng;
    rng.Seed(randomNum->getSeed());
    nn_gen->init(rng);
    NEAT::NeuralNetwork nn;
    nn_gen->buildPhenotype(nn);
    int nbr_weights = nn.m_connections.size();
    int nbr_bias = nn.m_neurons.size();
    int i = 0;
    for(; i < nbr_weights; i++)
        initial_point.push_back(nn.m_connections[i].m_weight);
    int j = 0;
    for(; i < nbr_weights+nbr_bias; i++){
        initial_point.push_back(nn.m_neurons[j].m_bias);
        j++;
    }

    cmaes::CMAParameters<> cmaParam(initial_point,step_size,pop_size,randomNum->getSeed());
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);

    cmaStrategy.reset(new customCMAStrategy([](const double*,const int&)->double{},cmaParam));
    cmaStrategy->set_elitist_restart(elitist_restart);
    cmaStrategy->set_length_of_stagnation(lenStag);

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
        Individual::Ptr ind(new CMAESIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }

}

void CMAES::epoch(){

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool withRestart = settings::getParameter<settings::Boolean>(parameters,"#withRestart").value;
    bool incrPop = settings::getParameter<settings::Boolean>(parameters,"#incrPop").value;


    int pop_size = cmaStrategy->get_parameters().lambda();

    cmaStrategy->set_population(population);
    cmaStrategy->eval();
    cmaStrategy->tell();
    if(withRestart && cmaStrategy->stop()){
        if(verbose)
            std::cout << "Restart !" << std::endl;

        cmaStrategy->capture_best_solution(best_run);
        int stop_type = cmaStrategy->get_solutions().run_status();
        if(stop_type == cmaes::FTARGET){
            _is_finish = true;
            return;
        }

        if(incrPop)
            cmaStrategy->lambda_inc();
        cmaStrategy->reset_search_state();
    }

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
        Individual::Ptr ind(new CMAESIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
    cmaStrategy->inc_iter();

}

void CMAES::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool CMAES::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    std::dynamic_pointer_cast<CMAESIndividual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());




    return true;
}

bool CMAES::is_finish(){
    int maxNbrEval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    return _is_finish || numberEvaluation >= maxNbrEval;
}
