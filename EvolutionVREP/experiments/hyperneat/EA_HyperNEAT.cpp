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

    startEvalTime = hr_clock::now();
}

void EA_HyperNEAT::initPopulation(const NEAT::Parameters &params)
{
    NEAT::Genome neat_genome(0, 5, 5, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 0);
    neat_population = std::make_unique<NEAT::Population>(neat_genome, params, true, 1.0, randomNum->getSeed());


    for (size_t i = 0; i < params.PopulationSize ; i++)
    {
        CPPNGenome::Ptr ctrlgenome(new CPPNGenome(neat_population->AccessGenomeByIndex(i)));
        EmptyGenome::Ptr no_gen(new EmptyGenome);

        CPPNIndividual::Ptr ind(new CPPNIndividual(no_gen,ctrlgenome));
        ind->set_individual_id(i);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);

        population.push_back(ind);
    }
}

void EA_HyperNEAT::setObjectives(size_t indIndex, const std::vector<double> &objectives){
    currentIndIndex = indIndex;
    population[indIndex]->setObjectives(objectives);
}


bool EA_HyperNEAT::update(const Environment::Ptr& env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    if(env->get_name() == "mazeEnv")
        std::dynamic_pointer_cast<CPPNIndividual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
    else if(env->get_name() == "testEnv")
        std::dynamic_pointer_cast<CPPNIndividual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<TestEnv>(env)->get_final_position());
    else std::cerr << "unknown environment" << std::endl;

    return true;
}

void EA_HyperNEAT::epoch(){
    for(int i = 0; i < population.size(); i++)
        neat_population->AccessGenomeByIndex(i).SetFitness(population[i]->getObjectives()[0]);
    neat_population->Epoch();
}

void EA_HyperNEAT::init_next_pop(){
    population.clear();
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    for (int i = 0; i < pop_size ; i++)
    {
        CPPNGenome::Ptr ctrlgenome(new CPPNGenome(neat_population->AccessGenomeByIndex(i)));
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        CPPNIndividual::Ptr ind(new CPPNIndividual(no_gen,ctrlgenome));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}
