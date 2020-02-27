#include "basicEA.hpp"

using namespace are;

void BasicEA::init(){

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    double max_weight = settings::getParameter<settings::Double>(parameters,"#maxWeight").value;

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


    rng.Seed(randomNum->getSeed());

    for(int i = 0; i < pop_size; i++){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome(randomNum,parameters));
        ctrl_gen->set_weights(randomNum->randVectd(-max_weight,max_weight,nbr_weights));
        ctrl_gen->set_biases(randomNum->randVectd(-max_weight,max_weight,nbr_bias));
        Individual::Ptr ind(new NN2Individual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void BasicEA::selection()
{
    //tournament selection

    assert(population.size()%2 == 0);

    unsigned int k = settings::getParameter<settings::Integer>(parameters,"#tournamentSize").value;

    childrens.clear();
    childrens.resize(population.size());
    int nbr_selected = 0;

    auto random_pick = [&]() -> std::vector<size_t>
    {
        std::vector<size_t> res;
        std::vector<size_t> idxes;
        for(size_t i = 0; i < population.size(); i++){
            idxes.push_back(i);
        }
        for(size_t i = 0; i < k; i++){
            int n = randomNum->randInt(0,idxes.size()-1);
            res.push_back(idxes[n]);
            idxes.erase(idxes.begin() + n);
            idxes.shrink_to_fit();
        }
        return res;
    };

    auto find_best_ind = [&](const std::vector<size_t> &ids) -> Individual::Ptr
    {
        double best_fit = population[ids[0]]->getObjectives()[0];
        Individual::Ptr best_ind = population[ids[0]];
        for(size_t id : ids){
            if(population[id]->getObjectives()[0] > best_fit){
                best_fit = population[id]->getObjectives()[0];
                best_ind = population[id];
            }
        }
        return best_ind;
    };
    //do tournament until reaching the size of the population
    std::vector<size_t> ids;
    while (nbr_selected < population.size()) {
        ids = random_pick();
        Individual::Ptr best_ind = find_best_ind(ids);
        childrens[nbr_selected] = best_ind->clone();
        nbr_selected++;
    }
}

    void BasicEA::crossover()
    {
        auto shuffle = [&]() -> std::vector<size_t>{
                std::vector<size_t> res;
                std::vector<size_t> idxes;
                for(size_t i = 0; i < childrens.size(); i++)
                idxes.push_back(i);

        for(size_t i = 0; i < childrens.size(); i++){
            int n = randomNum->randInt(0,idxes.size()-1);
            res.push_back(idxes[n]);
            idxes.erase(idxes.begin() + n);
            idxes.shrink_to_fit();
        }
        return res;
    };

    std::vector<size_t> c1 = shuffle();
    std::vector<size_t> c2 = shuffle();

    population.clear();

    for(int i = 0; i < c1.size()/2; i++){
        Individual::Ptr child1(new NN2Individual), child2(new NN2Individual);
        childrens[c1[i]]->crossover(childrens[c2[i]],*child1,*child2);
        population.push_back(child1);
        population.push_back(child2);
    }

    }

void BasicEA::mutation()
{
    for(Individual::Ptr &ind : population)
        ind->mutate();
}

void BasicEA::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool BasicEA::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
    }


    return true;
}

void BasicEA::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool BasicEA::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    std::dynamic_pointer_cast<NNIndividual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());


    return true;
}
