#include "noEA.h"

using namespace are;

void noEA::init(){
    NEAT::Parameters params = NEAT::Parameters();
    /// Set parameters for NEAT
    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    params.PopulationSize = pop_size;
    params.DynamicCompatibility = true;
    params.CompatTreshold = 2.0;
    params.YoungAgeTreshold = 15;
    params.SpeciesMaxStagnation = 100;
    params.OldAgeTreshold = 35;
    params.MinSpecies = 5;
    params.MaxSpecies = 10;
    params.RouletteWheelSelection = false;

    params.MutateRemLinkProb = 0.02;
    params.RecurrentProb = 0.0;
    params.OverallMutationRate = 0.15;
    params.MutateAddLinkProb = 0.08;
    params.MutateAddNeuronProb = 0.01;
    params.MutateWeightsProb = 0.90;
    params.MaxWeight = 8.0;
    params.WeightMutationMaxPower = 0.2;
    params.WeightReplacementMaxPower = 1.0;

    params.MutateActivationAProb = 0.0;
    params.ActivationAMutationMaxPower = 0.5;
    params.MinActivationA = 0.05;
    params.MaxActivationA = 6.0;

    NEAT::Genome neat_genome(0,9,16,2,false,NEAT::ActivationFunction::SIGNED_SIGMOID,NEAT::ActivationFunction::SIGNED_SIGMOID,1,params,1);
    std::unique_ptr<NEAT::Population> neat_population = std::make_unique<NEAT::Population>(neat_genome, params, true, 1.0, randomNum->getSeed());

    for(int i = 0; i < pop_size; i++){
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        NEATGenome::Ptr neatGen(new NEATGenome);
        neatGen->neat_genome = neat_population->AccessGenomeByIndex(i);
        BOLearner::Ptr learner(new BOLearner);
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
    o(0) = ind->getFitness();
    observations.push_back(o);

    auto connections = std::dynamic_pointer_cast<NNControl>(ind->get_control())->nn.m_connections;
    Eigen::VectorXd s(connections.size());
    int i = 0;
    for(const auto &conn : connections){
        s(i) = conn.m_weight;
        i++;
    }
    samples.push_back(s);

    if(verbose)
        std::cout << "Size of dataset for BO : " <<  observations.size() << std::endl;

    if(currentFitnesses.size() == nbr_bo_iter || generation == 0)
    {
        ind->setFitness(computeFitness());
        currentFitnesses.clear();
        ind.reset();

        return true;
    }

    std::dynamic_pointer_cast<BOIndividual>(ind)->update_learner(observations, samples);
    std::dynamic_pointer_cast<BOIndividual>(ind)->update_learner_model(observations, samples);

    ind.reset();
    return false;
}

void noEA::setFitness(size_t indIndex, float fitness){
    currentIndIndex = indIndex;
    currentFitnesses.push_back(fitness);
}

float noEA::computeFitness(){
    float fit;
    fit = currentFitnesses.back() + (currentFitnesses.back() - currentFitnesses.front());
    return fit;
}

void noEA::epoch(){
    EmptyGenome::Ptr no_gen(new EmptyGenome);
    BOLearner::Ptr learner(new BOLearner);
    BOIndividual::Ptr ind(new BOIndividual(no_gen,no_gen,learner));
    ind->set_parameters(parameters);
    ind->initRandNum(randomNum->getSeed());
    population.push_back(ind);
}


