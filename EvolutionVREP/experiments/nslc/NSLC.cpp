#include "NSLC.hpp"


using namespace are;

void NSLC::init(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    double max_weight = settings::getParameter<settings::Double>(parameters,"#maxWeight").value;

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


    nb_obj = 2;
    max_obj = {1,1};
    min_obj = {0,0};

    for(int i = 0; i < pop_size; i++){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome(randomNum,parameters));
        ctrl_gen->set_weights(randomNum->randVectd(-max_weight,max_weight,nbr_weights));
        ctrl_gen->set_biases(randomNum->randVectd(-max_weight,max_weight,nbr_bias));
        NSLCIndividual::Ptr ind(new NSLCIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void NSLC::epoch(){

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
        pop_desc.push_back(std::dynamic_pointer_cast<NSLCIndividual>(ind)->descriptor());
    std::vector<Eigen::VectorXd> pop_desc;
    for(const auto& ind : population)
        pop_desc.push_back(std::dynamic_pointer_cast<NSLCIndividual>(ind)->descriptor());

    //compute novelty and local competition scores
    for(const auto& ind : population){
        Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<NSLCIndividual>(ind)->descriptor();
        //Compute distances to find the k nearest neighbors of ind
        std::vector<size_t> pop_indexes;
        std::vector<double> distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes);

        //Compute novelty
        double ind_nov = Novelty::sparseness(distances);

        //Compute local competition score
        double lc_score = 0;
        for(size_t i = 0; i < Novelty::k_value; i++){
            if(ind->getObjectives()[0] > population[pop_indexes[i]]->getObjectives()[0]) //fitness of the individual
                lc_score += 1;
        }
        lc_score /= Novelty::k_value;

        //set the objetives
        std::vector<double> objectives = {lc_score,ind_nov};
        ind->setObjectives(objectives);
    }

    //update archive for novelty score
    for(const auto& ind : population){
        Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<NSLCIndividual>(ind)->descriptor();
        double ind_nov = ind->getObjectives().back();
        Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
    }

    /**/


    NSGA2::epoch();
}

void NSLC::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool NSLC::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());


    return true;
}

bool NSLC::is_finish(){
    double ftarget = settings::getParameter<settings::Double>(parameters,"#fTarget").value;
    double max_eval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    return best_fitness > ftarget || numberEvaluation > max_eval;
}


