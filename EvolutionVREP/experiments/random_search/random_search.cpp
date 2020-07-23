#include "random_search.hpp"

using namespace are;

void RandomSearch::init(){
    int nbr_evaluation = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;

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

    Eigen::MatrixXd init_samples = limbo::tools::random_lhs(nbr_weights + nbr_biases,nbr_evaluation);

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_biases);

    for(int i = 0; i < nbr_evaluation; i++){
        for(int v = 0; v < nbr_weights; v++)
            weights[v] = max_weight*(init_samples(i,v)*2.f - 1.f);
        for(int w = nbr_weights; w < nbr_weights+nbr_biases; w++)
            biases[w-nbr_weights] = max_weight*(init_samples(i,w)*2.f - 1.f);

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


void RandomSearch::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}


bool RandomSearch::update(const Environment::Ptr& env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_trajectory(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_trajectory());
    }


    return true;
}

bool RandomSearch::is_finish(){
    int nbr_evaluations = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    return numberEvaluation >= nbr_evaluations;
}
