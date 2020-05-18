#include "BOCMAES.hpp"


using namespace are;


void BOCMAES::init(){

    int init_bo_dataset = settings::getParameter<settings::Integer>(parameters,"#initBODataSet").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;

    Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProb").value;

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;

    int nbr_weights, nbr_biases;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_biases);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_biases);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_biases);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }


    Eigen::MatrixXd init_samples = limbo::tools::random_lhs(nbr_weights + nbr_biases,init_bo_dataset);

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_biases);

    for(int u = 0; u < init_bo_dataset; u++){

        for(int v = 0; v < nbr_weights; v++)
            weights[v] = max_weight*(init_samples(u,v)*2.f - 1.f);
        for(int w = nbr_weights; w < nbr_weights+nbr_biases; w++)
            biases[w-nbr_weights] = max_weight*(init_samples(u,w)*2.f - 1.f);

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


    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;
    int nbr_weights, nbr_biases;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_biases);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_biases);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_biases);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }


    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_biases);

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

    double lb[nbr_weights+nbr_biases], ub[nbr_weights+nbr_biases];
    for(int i = 0; i < nbr_weights+nbr_biases; i++){
        lb[i] = -max_weight;
        ub[i] = max_weight;
    }
    geno_pheno_t gp(lb,ub,nbr_weights+nbr_biases);


    cmaes::CMAParameters<geno_pheno_t> cmaParam(initial_point,step_size,pop_size,randomNum->getSeed(),gp);
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);

    cmaStrategy.reset(new IPOPCMAStrategy([](const double*,const int&)->double{},cmaParam));
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
            Individual::Ptr ind = population[rnd_i];
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
        NIPES::epoch();
    }
}

void BOCMAES::bo_epoch(){
    Individual::Ptr ind = population[currentIndIndex];


    if(generation == 0){
        std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->compute_model(observations,samples);
    }

    Eigen::VectorXd target(3);
    target << settings::getParameter<settings::Double>(parameters,"#target_x").value,
            settings::getParameter<settings::Double>(parameters,"#target_y").value,
            settings::getParameter<settings::Double>(parameters,"#target_z").value;

    std::dynamic_pointer_cast<BOCMAESIndividual>(ind)->update_learner(observations, samples,target);

    ind.reset();
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



