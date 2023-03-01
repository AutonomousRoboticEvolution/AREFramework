#include "NIPES.hpp"


using namespace are;

Eigen::VectorXd NIPESIndividual::descriptor()
{
    if(descriptor_type == FINAL_POSITION){
        double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
        Eigen::VectorXd desc(3);
        desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
        return desc;
    }else if(descriptor_type == VISITED_ZONES){
        Eigen::MatrixXd vz = visited_zones.cast<double>();
        Eigen::VectorXd desc(Eigen::Map<Eigen::VectorXd>(vz.data(),vz.cols()*vz.rows()));
        return desc;
    }
}

std::string NIPESIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<NIPESIndividual>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void NIPESIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<NIPESIndividual>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}

int NIPES::novelty_params::k_value = 15;
double NIPES::novelty_params::archive_adding_prob = 0.4;
double NIPES::novelty_params::novelty_thr = 0.9;

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

    novelty_params::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;
    novelty_params::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
    novelty_params::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProb").value;

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;
    const std::vector<int> joint_subs = settings::getParameter<settings::Sequence<int>>(parameters,"#jointSubs").value;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN_CPG)
        NN2Control<elman_cpg_t>::nbr_parameters_cpg(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias,joint_subs);
    else if(nn_type == settings::nnType::CPG)
        NN2Control<cpg_t>::nbr_parameters_cpg(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias,joint_subs);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

    std::string bootstrapCtrl = settings::getParameter<settings::String>(parameters,"#bootstrapControllerFile").value;
    std::vector<double> initial_point;
    if(bootstrapCtrl != "None"){
        NNParamGenome ctrl_gen;
        ctrl_gen.from_file(bootstrapCtrl);
        initial_point = ctrl_gen.get_full_genome();
    }else initial_point = randomNum->randVectd(-max_weight,max_weight,nbr_weights + nbr_bias);

    double lb[nbr_weights+nbr_bias], ub[nbr_weights+nbr_bias];
    for(int i = 0; i < nbr_weights+nbr_bias; i++){
        lb[i] = -max_weight;
        ub[i] = max_weight;
    }

    geno_pheno_t gp(lb,ub,nbr_weights+nbr_bias);

    cma::CMAParameters<geno_pheno_t> cmaParam(initial_point,step_size,pop_size,randomNum->getSeed(),gp);
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);


    _cma_strat.reset(new IPOPCMAStrategy([](const double*,const int&)->double{},cmaParam));
    _cma_strat->set_elitist_restart(elitist_restart);
    _cma_strat->set_length_of_stagnation(lenStag);
    _cma_strat->set_novelty_ratio(novelty_ratio);
    _cma_strat->set_novelty_decr(novelty_decr);
    _cma_strat->set_pop_stag_thres(pop_stag_thres);

    if(bootstrapCtrl == "None"){
        std::string learnerfile = settings::getParameter<settings::String>(parameters,"#learnerFile").value;
        if(learnerfile != "None"){
            _cma_strat->from_file(learnerfile);
            pop_size =  _cma_strat->get_parameters().lambda();
        }
    }

    dMat init_samples = _cma_strat->ask();

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
        ctrl_gen->set_nbr_output(nb_output);
        ctrl_gen->set_nbr_input(nb_input);
        ctrl_gen->set_nbr_hidden(nb_hidden);
        ctrl_gen->set_nn_type(nn_type);
        Individual::Ptr ind(new NIPESIndividual(morph_gen,ctrl_gen));
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
            double ec = std::dynamic_pointer_cast<sim::NN2Individual>(ind)->get_energy_cost();
            if(ec > energy_budget) ec = energy_budget;
            std::dynamic_pointer_cast<sim::NN2Individual>(ind)->addObjective(1 - ec/energy_budget);
        }
    }

    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(novelty_params::k_value >= population.size())
            novelty_params::k_value = population.size()/2;
        else novelty_params::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for(const auto& ind : population)
            pop_desc.push_back(ind->descriptor());
        //compute novelty
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = ind->descriptor();
            double ind_nov = novelty::sparseness<novelty_params>(Novelty::distances(ind_desc,archive,pop_desc));
            std::dynamic_pointer_cast<sim::NN2Individual>(ind)->addObjective(ind_nov);
        }

        //update archive
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = ind->descriptor();
            double ind_nov = ind->getObjectives().back();
            novelty::update_archive<novelty_params>(ind_desc,ind_nov,archive,randomNum);
        }
    }
    /**/

    std::vector<IPOPCMAStrategy::individual_t> pop;
    for(const auto &ind : population){
        IPOPCMAStrategy::individual_t cma_ind;
        cma_ind.genome = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();
        cma_ind.descriptor = std::dynamic_pointer_cast<sim::NN2Individual>(ind)->get_final_position();
        cma_ind.objectives = std::dynamic_pointer_cast<sim::NN2Individual>(ind)->getObjectives();
        pop.push_back(cma_ind);
    }

    _cma_strat->set_population(pop);
    _cma_strat->eval();
    _cma_strat->tell();
    bool stop = _cma_strat->stop();
//    if(_cma_strat->have_reached_ftarget()){
//        _is_finish = true;
////        return;
//    }

    if(withRestart && stop){
        if(verbose)
            std::cout << "Restart !" << std::endl;

        _cma_strat->capture_best_solution(best_run);

        if(incrPop)
            _cma_strat->lambda_inc();

        _cma_strat->reset_search_state();
        if(!elitist_restart){
            float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
            _cma_strat->get_parameters().set_x0(-max_weight,max_weight);
        }
    }
}

void NIPES::init_next_pop(){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;
    int pop_size = _cma_strat->get_parameters().lambda();

    dMat new_samples = _cma_strat->ask();

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
        ctrl_gen->set_nbr_output(nb_output);
        ctrl_gen->set_nbr_input(nb_input);
        ctrl_gen->set_nbr_hidden(nb_hidden);
        ctrl_gen->set_nn_type(nn_type);
        Individual::Ptr ind(new NIPESIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void NIPES::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    population[indIdx]->setObjectives(objectives);
}


bool NIPES::update(const Environment::Ptr & env){
    numberEvaluation++;
    reevaluated++;

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<NIPESIndividual>(ind)->set_final_position(env->get_final_position());
        std::dynamic_pointer_cast<NIPESIndividual>(ind)->set_trajectory(env->get_trajectory());
        if(env->get_name() == "obstacle_avoidance"){
            std::dynamic_pointer_cast<NIPESIndividual>(ind)->set_visited_zones(std::dynamic_pointer_cast<sim::ObstacleAvoidance>(env)->get_visited_zone_matrix());
            std::dynamic_pointer_cast<NIPESIndividual>(ind)->set_descriptor_type(VISITED_ZONES);

	}
	else if(env->get_name() == "exploration"){
            std::dynamic_pointer_cast<NIPESIndividual>(ind)->set_visited_zones(std::dynamic_pointer_cast<sim::Exploration>(env)->get_visited_zone_matrix());
            std::dynamic_pointer_cast<NIPESIndividual>(ind)->set_descriptor_type(VISITED_ZONES);
        }
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

bool NIPES::finish_eval(const Environment::Ptr & env){

    if(env->get_name() == "obstacle_avoidance" || env->get_name() == "exploration")
        return false;

    std::vector<double> target = settings::getParameter<settings::Sequence<double>>(parameters,"#targetPosition").value;
    float t_pos[3] = {target[0],target[1],target[2]};
    double fTarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    double arenaSize = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;

    auto distance = [](float* a,float* b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };

    int handle = std::dynamic_pointer_cast<sim::Morphology>(population[currentIndIndex]->get_morphology())->getMainHandle();
    float pos[3];
    simGetObjectPosition(handle,-1,pos);
    double dist = distance(pos,t_pos)/sqrt(2*arenaSize*arenaSize);

    if(dist < fTarget){
        std::cout << "STOP !" << std::endl;
    }

    return  dist < fTarget;
}

void NIPES::update_pop_info(const std::vector<double> &obj, const Eigen::VectorXd &desc){
    int idx = _cma_strat->get_population().size();
    IPOPCMAStrategy::individual_t ind;
    ind.genome = std::dynamic_pointer_cast<NNParamGenome>(population[idx]->get_ctrl_genome())->get_full_genome();
    ind.objectives = obj;
    ind.descriptor.resize(desc.rows());
    for(int i = 0; i < desc.rows(); i++)
        ind.descriptor[i] = desc(i);
    _cma_strat->add_individual(ind);
}
