#include "NSGC.hpp"


using namespace are;

void NSGC::init(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    double max_weight = settings::getParameter<settings::Double>(parameters,"#maxWeight").value;
    bool with_novelty = settings::getParameter<settings::Boolean>(parameters,"#withNovelty").value;

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

    if(with_novelty){
        nb_obj = 2;
        max_obj = {1,1};
        min_obj = {0,0};
    }else{
        nb_obj = 1;
        max_obj = {1};
        min_obj = {0};
    }

    for(int i = 0; i < pop_size; i++){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome(randomNum,parameters));
        ctrl_gen->set_weights(randomNum->randVectd(-max_weight,max_weight,nbr_weights));
        ctrl_gen->set_biases(randomNum->randVectd(-max_weight,max_weight,nbr_bias));
        NSGCIndividual::Ptr ind(new NSGCIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void NSGC::epoch(){

    bool with_novelty = settings::getParameter<settings::Boolean>(parameters,"#withNovelty").value;

    best_fitness = 0;
    for(const Individual::Ptr &ind : population){
        if(ind->getObjectives()[0] > best_fitness)
            best_fitness = ind->getObjectives()[0];
    }

    /** NOVELTY **/
    if(Novelty::k_value >= population.size())
        Novelty::k_value = population.size()/2;
    else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

    std::vector<Eigen::VectorXd> pop_desc;
    for(const auto& ind : population)
        pop_desc.push_back(std::dynamic_pointer_cast<NSGCIndividual>(ind)->descriptor());

    //compute novelty and local competition scores
    if(with_novelty){
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<NSGCIndividual>(ind)->descriptor();
            //Compute distances to find the k nearest neighbors of ind
            std::vector<size_t> pop_indexes;
            std::vector<double> distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes);

            //Compute novelty
            double ind_nov = Novelty::sparseness(distances);

            //set the objetives
            std::dynamic_pointer_cast<NSGCIndividual>(ind)->addObjective(ind_nov);
        }

        //update archive for novelty score
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<NSGCIndividual>(ind)->descriptor();
            double ind_nov = ind->getObjectives().back();
            Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
        }
    }
    /**/

    NSGA2::epoch();
}

void NSGC::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool NSGC::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];
    std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());

    return true;
}

bool NSGC::is_finish(){
    double ftarget = settings::getParameter<settings::Double>(parameters,"#fTarget").value;
    double max_eval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    return best_fitness > ftarget || numberEvaluation > max_eval;
}
<<<<<<< HEAD

=======
>>>>>>> add new experiment Novelty search with global competition
