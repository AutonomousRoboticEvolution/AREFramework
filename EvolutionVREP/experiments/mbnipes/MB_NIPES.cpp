#include "MB_NIPES.hpp"


using namespace are;

obs_fct_t obs_fcts::final_position = [](const NN2Individual::Ptr &ind) -> Eigen::VectorXd{
    Eigen::VectorXd obs(3);
    std::vector<double> final_pos = ind->get_final_position();
    obs << final_pos[0], final_pos[1], final_pos[2];
    return obs;
};

obs_fct_t obs_fcts::reward = [](const NN2Individual::Ptr &ind) -> Eigen::VectorXd{
    Eigen::VectorXd obs(1);
    obs << ind->getObjectives()[0];
    return obs;
};

void MB_NIPES::init(){
    NIPES::init();
    _compute_obs = obs_fcts::final_position;
    learner.reset(new BOLearner(parameters));

}

void MB_NIPES::epoch(){

    for(Individual::Ptr &ind : virtual_pop){
        std::vector<double> gen = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();
        Eigen::VectorXd mu = learner->get_mu(gen);

        std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position({mu(0),mu(1),mu(2)});
        std::dynamic_pointer_cast<NN2Individual>(ind)->addObjective(learner->reward(mu));
        population.push_back(ind);
    }
    virtual_pop.clear();

    NIPES::epoch();
    learner->set_observation(observations);
    learner->set_samples(samples);
    if(generation == 0)
        learner->compute_model();
    else learner->update_model();
}

void MB_NIPES::init_next_pop(){

    double uncertainty_threshold = settings::getParameter<settings::Double>(parameters,"#uncertaintyThreshold").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

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
        double sigma =  learner->get_sigma(ctrl_gen->get_full_genome());
        if(sigma < uncertainty_threshold)
            virtual_pop.push_back(ind);
        else
            population.push_back(ind);
    }
    if(verbose)
        std::cout << virtual_pop.size() << " not evaluated over " << pop_size << std::endl;

}

bool MB_NIPES::update(const Environment::Ptr &env){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    numberEvaluation++;
    Individual::Ptr ind = population[currentIndIndex];

    if(simulator_side){
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
    }

    if(instance_type == settings::INSTANCE_REGULAR || !simulator_side){

        observations.push_back(_compute_obs(std::dynamic_pointer_cast<NN2Individual>(ind)));

        std::vector<double> nn_params = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();
        Eigen::VectorXd s(nn_params.size());
        for(size_t i = 0; i < nn_params.size(); i++)
            s(i) = nn_params[i];

        samples.push_back(s);


//        if(verbose)
//            std::cout << "Size of dataset for BO : " <<  observations.size() << std::endl;

        ind.reset();
    }
    return true;
}
