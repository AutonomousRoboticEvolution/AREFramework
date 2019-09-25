/**
	@file EA_MultiNEAT.cpp
    @authors Matteo de Carlo and Edgar Buchanan
	@brief This class handles evolution using MultiNEAT.
*/

#include "EA_MultiNEAT.h"
#include "EA_MultiNEATGenome.h"
#include <memory>

EA_MultiNEAT::EA_MultiNEAT()
{
    if (not settings or settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
}

EA_MultiNEAT::~EA_MultiNEAT()
{
    if (not settings or settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
}

/// Initialiase EA.
void EA_MultiNEAT::init()
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;

    if (settings->verbose) std::cout << "Initialising Multi-NEAT" << std::endl;

    NEAT::Parameters params = NEAT::Parameters();
    /// Set parameters for NEAT
    params.PopulationSize = settings->populationSize;
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

    params.MutateNeuronTraitsProb = 0.0;
    params.MutateLinkTraitsProb = 0.0;

    params.AllowLoops = false;


    this->initializePopulation(params);
}

void EA_MultiNEAT::initializePopulation(const NEAT::Parameters &params)
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
    /// Defines the initial conditions of the genome
    // (?, number of inputs, number if hidden neurons/layers?, number of outputs, ?, output activation function,
    // (hidden activation function, ?, parameters, number of layers)
    NEAT::Genome neat_genome(0, 3, 3, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 0);
    population = std::make_unique<NEAT::Population>(neat_genome, params, true, 1.0, randomNum->getSeed());

    /// Create population of genomes
    for (int i = 0; i < settings->populationSize; i++)
    {
        std::shared_ptr<Genome> genome = std::make_shared<EA_MultiNEATGenome>(population->AccessGenomeByIndex(i));
        nextGenGenomes.emplace_back(std::move(genome));
        nextGenGenomes[i]->fitness = 0;
        nextGenGenomes[i]->individualNumber = i;
    }

    /// From here and on the code should be else where.
//    for(int i = 1; i <= 20; i++){
//        double bestf = -std::numeric_limits<double>::infinity();
//        for(unsigned int j = 0; j < population->m_Species.size(); j++){
//            for(unsigned int k = 0; k < population->m_Species[j].m_Individuals.size(); k++){
//                population->m_Species[j].m_Individuals[k].SetFitness(randomNum->randFloat(0,1));
//                population->m_Species[j].m_Individuals[k].SetEvaluated();
//                std::vector<double> inputs {0.0,1.0};
//                NEAT::NeuralNetwork neuralNetwork;
//                genome.BuildPhenotype(neuralNetwork);
//                neuralNetwork.Input(inputs);
//                neuralNetwork.Activate();
//                double result = neuralNetwork.Output()[0];
//                std::cout << result << std::endl;
//            }
//        }
//        population->Epoch();
//    }
}

void EA_MultiNEAT::epoch()
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
    this->population->Epoch();
    this->nextGenGenomes.clear();
    for (int i = 0; i < settings->populationSize; i++)
    {
        std::shared_ptr<Genome> genome = std::make_shared<EA_MultiNEATGenome>(population->AccessGenomeByIndex(i));
        nextGenGenomes.emplace_back(std::move(genome));
        nextGenGenomes[i]->fitness = 0;
        nextGenGenomes[i]->individualNumber = static_cast<int>(population->GetGeneration() * settings->populationSize) + i;
    }
}

void EA_MultiNEAT::replacement()
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
}

void EA_MultiNEAT::selection()
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
    this->epoch();
}

void EA_MultiNEAT::mutation()
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
}

void EA_MultiNEAT::update()
{
//    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
}

void EA_MultiNEAT::end()
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
}

void EA_MultiNEAT::createIndividual(int ind)
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << '(' << ind << ')' << std::endl;
    this->nextGenGenomes[ind]->init();
    this->morph = this->nextGenGenomes[ind]->morph;
}

void EA_MultiNEAT::loadBestIndividualGenome(int sceneNum)
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
}

void EA_MultiNEAT::setFitness(int ind, float fit)
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << '(' << ind << ',' << fit << ')' << std::endl;
    this->population->AccessGenomeByIndex(ind).SetFitness(fit);
}

std::shared_ptr<Morphology> EA_MultiNEAT::getMorph()
{
    if (settings->verbose) std::cerr << "EA_MultiNEAT::" << __func__ << std::endl;
    assert(this->morph);
    return this->morph;
}

void EA_MultiNEAT::savePopulation(const std::string &filename)
{
    this->population->Save(filename.c_str());
}
