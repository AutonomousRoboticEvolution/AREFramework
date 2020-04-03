#include "basicEA.hpp"

using namespace are;

void BasicEA::init(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    rng.Seed(randomNum->getSeed());

    for(int i = 0; i < pop_size; i++){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        NNGenome::Ptr ctrl_gen(new NNGenome(randomNum,parameters));
        ctrl_gen->init(rng);
        Individual::Ptr ind(new NNIndividual(morph_gen,ctrl_gen));
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

void BasicEA::replacement()
{
    population.clear();
    population = childrens;
    population.resize(childrens.size());
    population.shrink_to_fit();
}

void BasicEA::crossover()
{
    //No crossover
}

void BasicEA::mutation()
{
    for(Individual::Ptr &ind : population)
        std::dynamic_pointer_cast<NNGenome>(ind->get_ctrl_genome())->mutate(rng);
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

