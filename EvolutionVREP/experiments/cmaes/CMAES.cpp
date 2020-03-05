#include "CMAES.hpp"


using namespace are;

void CMAES::init(){

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    std::vector<double> initial_point;

    NNGenome::Ptr nn_gen(new NNGenome(randomNum,parameters));
    NEAT::RNG rng;
    rng.Seed(randomNum->getSeed());
    nn_gen->init(rng);
    NEAT::NeuralNetwork nn;
    nn_gen->get_nn_genome().BuildPhenotype(nn);
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


    double step_size = settings::getParameter<settings::Double>(parameters,"#CMAESStep").value;

    cmaes::CMAParameters<> cmaParam(initial_point,step_size,pop_size,randomNum->getSeed());

    cmaStrategy.reset(new customCMAStrategy([](const double*,const int&)->double{},cmaParam));

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

    int pop_size = population.size();

    cmaStrategy->set_population(population);
    cmaStrategy->eval();
    cmaStrategy->tell();

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
