#include "EA_HyperNEAT.h"

using namespace are;

void EA_HyperNEAT::init()
{
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

    params.MutateNeuronActivationTypeProb = 0.03;

    params.ActivationFunction_SignedSigmoid_Prob = 0.0;
    params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
    params.ActivationFunction_Tanh_Prob = 1.0;
    params.ActivationFunction_TanhCubic_Prob = 0.0;
    params.ActivationFunction_SignedStep_Prob = 1.0;
    params.ActivationFunction_UnsignedStep_Prob = 0.0;
    params.ActivationFunction_SignedGauss_Prob = 1.0;
    params.ActivationFunction_UnsignedGauss_Prob = 0.0;
    params.ActivationFunction_Abs_Prob = 0.0;
    params.ActivationFunction_SignedSine_Prob = 1.0;
    params.ActivationFunction_UnsignedSine_Prob = 0.0;
    params.ActivationFunction_Linear_Prob = 1.0;

    initPopulation(params);

}

void EA_HyperNEAT::initPopulation(const NEAT::Parameters &params)
{
    NEAT::Genome neat_genome(0, 4, 5, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 0);
    neat_population = std::make_unique<NEAT::Population>(neat_genome, params, true, 1.0, randomNum->getSeed());

    for (size_t i = 0; i < params.PopulationSize ; i++)
    {
        CPPNGenome::Ptr ctrlgenome(new CPPNGenome(neat_population->AccessGenomeByIndex(i)));
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        BOLearner::Ptr learner(new BOLearner);

        CPPNIndividual::Ptr ind(new CPPNIndividual(no_gen,ctrlgenome,learner));
        ind->set_individual_id(i);
        ind->set_parameters(parameters);
//        ind->init();
        population.push_back(ind);

        ctrlgenome.reset();
        no_gen.reset();
        learner.reset();
    }
}



bool EA_HyperNEAT::update()
{
    Individual::Ptr ind = population[currentIndIndex];
    for(auto & obs : std::dynamic_pointer_cast<CPPNIndividual>(ind)->get_observations())
        observations.push_back(obs);

    if(currentFitnesses.size() == 2)
    {
        population[currentIndIndex]->setFitness(computeFitness());
        currentFitnesses.clear();
        ind.reset();
        return true;
    }

    std::dynamic_pointer_cast<CPPNIndividual>(ind)->update_learner(observations);


    ind.reset();
    return false;
}

void EA_HyperNEAT::setFitness(size_t indIndex, float fitness){
    currentIndIndex = indIndex;
    currentFitnesses.push_back(fitness);
}

void EA_HyperNEAT::epoch(){
    for(int i = 0; i < population.size(); i++)
        neat_population->AccessGenomeByIndex(i).SetFitness(population[i]->getFitness());
    neat_population->Epoch();
    population.clear();
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    for (int i = 0; i < pop_size ; i++)
    {
        CPPNGenome::Ptr ctrlgenome(new CPPNGenome(neat_population->AccessGenomeByIndex(i)));
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        BOLearner::Ptr learner(new BOLearner);

        CPPNIndividual::Ptr ind(new CPPNIndividual(no_gen,ctrlgenome,learner));
        ind->set_parameters(parameters);
        population.push_back(ind);


        ctrlgenome.reset();
        no_gen.reset();
        learner.reset();
    }
}
