#include "CMABO.hpp"


using namespace are;

obs_fct_t obs_fcts::final_position = [](const NN2Individual::Ptr &ind) -> Eigen::VectorXd{
    Eigen::VectorXd obs(3);
    std::vector<double> final_pos = ind->get_final_position();
    obs << final_pos[0], final_pos[1], final_pos[2];
    return obs;
};


void CMABO::init(){
    NIPES::init();
    _compute_obs = obs_fcts::final_position;
    learner.reset(new BOLearner(parameters));
}

void CMABO::epoch(){
    NIPES::epoch();
    learner->set_observation(observations);
    learner->set_samples(samples);
    if(generation == 0)
        learner->compute_model();
    else learner->update_model();
}

void CMABO::init_next_pop(){

    NIPES::init_next_pop();

    NNParamGenome::Ptr bo_gen(new NNParamGenome);
    int nbr_weights = std::dynamic_pointer_cast<NNParamGenome>(population[0]->get_ctrl_genome())->get_weights().size();
    int nbr_bias = std::dynamic_pointer_cast<NNParamGenome>(population[0]->get_ctrl_genome())->get_biases().size();
    std::vector<double> w(nbr_weights),b(nbr_bias);
    bo_gen->set_biases(b);
    bo_gen->set_weights(w);
    learner->update(bo_gen);
    EmptyGenome::Ptr morph_gen(new EmptyGenome);
    Individual::Ptr ind(new NN2Individual(morph_gen,bo_gen));
    ind->set_parameters(parameters);
    ind->set_randNum(randomNum);
    population.push_back(ind);
}

bool CMABO::update(const Environment::Ptr &env){
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


        if(verbose)
            std::cout << "Size of dataset for BO : " <<  observations.size() << std::endl;

        ind.reset();
    }
    return true;
}
