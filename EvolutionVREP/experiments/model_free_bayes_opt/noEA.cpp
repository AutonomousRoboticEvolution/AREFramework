#include "noEA.h"

using namespace are;

void noEA::init(){
    /// Set parameters for NEAT
    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    neat_params.PopulationSize = pop_size;
    neat_params.DynamicCompatibility = true;
    neat_params.CompatTreshold = 2.0;
    neat_params.YoungAgeTreshold = 15;
    neat_params.SpeciesMaxStagnation = 100;
    neat_params.OldAgeTreshold = 35;
    neat_params.MinSpecies = 5;
    neat_params.MaxSpecies = 10;
    neat_params.RouletteWheelSelection = false;

    neat_params.MutateRemLinkProb = 0.02;
    neat_params.RecurrentProb = 0.0;
    neat_params.OverallMutationRate = 0.15;
    neat_params.MutateAddLinkProb = 0.08;
    neat_params.MutateAddNeuronProb = 0.01;
    neat_params.MutateWeightsProb = 0.90;
    neat_params.MaxWeight = 8.0;
    neat_params.WeightMutationMaxPower = 0.2;
    neat_params.WeightReplacementMaxPower = 1.0;

    neat_params.MutateActivationAProb = 0.0;
    neat_params.ActivationAMutationMaxPower = 0.5;
    neat_params.MinActivationA = 0.05;
    neat_params.MaxActivationA = 6.0;

    NEAT::Genome neat_genome(0,9,16,2,false,NEAT::ActivationFunction::SIGNED_SIGMOID,NEAT::ActivationFunction::SIGNED_SIGMOID,1,neat_params,1);
    std::unique_ptr<NEAT::Population> neat_population = std::make_unique<NEAT::Population>(neat_genome,neat_params, true, 1.0, randomNum->getSeed());

    for(int i = 0; i < pop_size; i++){
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        NEATGenome::Ptr neatGen(new NEATGenome);
        neatGen->neat_genome = neat_population->AccessGenomeByIndex(i);
        BOLearner::Ptr learner(new BOLearner);
        learner->set_parameters(parameters);
    	BOIndividual::Ptr ind(new BOIndividual(no_gen,neatGen,learner));
        ind->set_parameters(parameters);
        ind->initRandNum(randomNum->getSeed());
        population.push_back(ind);
    }


}

bool noEA::update()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int nbr_bo_iter = settings::getParameter<settings::Integer>(parameters,"#numberBOIteration").value;


    Individual::Ptr ind = population[currentIndIndex];

    //add last fitness and NN weights in the database for the Bayesian optimizer
    Eigen::VectorXd o(1);
    o(0) = currentFitnesses.back();
    observations.push_back(o);

    auto connections = std::dynamic_pointer_cast<NNControl>(ind->get_control())->nn.m_connections;
    auto neurons = std::dynamic_pointer_cast<NNControl>(ind->get_control())->nn.m_neurons;
    Eigen::VectorXd s(connections.size() + neurons.size());
    int i = 0;
    for(const auto &conn : connections){
        s(i) = conn.m_weight;
        i++;
    }
    for(const auto &neu : neurons){
        s(i) = neu.m_bias;
        i++;
    }

    samples.push_back(s);

    if(verbose)
        std::cout << "Size of dataset for BO : " <<  observations.size() << std::endl;

    if(currentFitnesses.size() == nbr_bo_iter || generation == 0)
    {
        currentFitnesses.clear();
        ind.reset();

        return true;
    }

    if(currentFitnesses.size() == 1){
        std::dynamic_pointer_cast<BOIndividual>(ind)->compute_model(observations,samples);
    }

    std::dynamic_pointer_cast<BOIndividual>(ind)->update_learner(observations, samples);

    ind.reset();
    return false;
}

void noEA::setFitness(size_t indIndex, float fitness){
    currentIndIndex = indIndex;
    currentFitnesses.push_back(fitness);
}


void noEA::epoch(){

    int rnd_i = randomNum->randInt(0,population.size());
    BOIndividual::Ptr ind = population[rnd_i];

    population.clear();

    population.push_back(ind);
}

//EmptyGenome::Ptr no_gen(new EmptyGenome);
//NEATGenome::Ptr neatGen(new NEATGenome);
//NEAT::Genome neat_genome(0,9,16,2,false,NEAT::ActivationFunction::SIGNED_SIGMOID,NEAT::ActivationFunction::SIGNED_SIGMOID,1,neat_params,1);
//neatGen->neat_genome = neat_genome;
//BOLearner::Ptr learner(new BOLearner);
//learner->set_parameters(parameters);
//BOIndividual::Ptr ind(new BOIndividual(no_gen,neatGen,learner));
//ind->set_parameters(parameters);
//ind->initRandNum(randomNum->getSeed());
