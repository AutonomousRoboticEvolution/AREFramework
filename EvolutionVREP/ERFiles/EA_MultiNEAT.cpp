/**
	@file EA_MultiNEAT.cpp
    @authors Matteo de Carlo and Edgar Buchanan
	@brief This class handles evolution using MultiNEAT.
*/

#include "EA_MultiNEAT.h"
#include <memory>

EA_MultiNEAT::EA_MultiNEAT() {

}

EA_MultiNEAT::~EA_MultiNEAT() {

}

/// Initialiase EA.
void EA_MultiNEAT::init() {
    if(settings->verbose){
        std::cout << "Initialising Multi-NEAT" << std::endl;
    }
    NEAT::Parameters params = NEAT::Parameters();
    /// Set parameters for NEAT
    params.PopulationSize = settings->populationSize;
    params.DynamicCompatibility = true;
    params.NormalizeGenomeSize = true;
    params.WeightDiffCoeff = 0.1;
    params.CompatTreshold = 2.0;
    params.YoungAgeTreshold = 15;
    params.SpeciesMaxStagnation = 15;
    params.OldAgeTreshold = 35;
    params.MinSpecies = 2;
    params.MaxSpecies = 4;
    params.RouletteWheelSelection = false;
    params.RecurrentProb = 0.0;
    params.OverallMutationRate = 1.0;

    params.ArchiveEnforcement = false;

    params.MutateWeightsProb = 0.05;

    params.WeightMutationMaxPower = 0.5;
    params.WeightReplacementMaxPower = 8.0;
    params.MutateWeightsSevereProb = 0.0;
    params.WeightMutationRate = 0.25;
    params.WeightReplacementRate = 0.9;

    params.MaxWeight = 8;

    params.MutateAddNeuronProb = 0.001;
    params.MutateAddLinkProb = 0.3;
    params.MutateRemLinkProb = 0.0;

    params.MinActivationA  = 4.9;
    params.MaxActivationA  = 4.9;

    params.ActivationFunction_SignedSigmoid_Prob = 0.0;
    params.ActivationFunction_UnsignedSigmoid_Prob = 1.0;
    params.ActivationFunction_Tanh_Prob = 0.0;
    params.ActivationFunction_SignedStep_Prob = 0.0;

    params.CrossoverRate = 0.0 ;
    params.MultipointCrossoverRate = 0.0;
    params.SurvivalRate = 0.2;

    params.AllowClones = true;
    params.AllowLoops = true;

    params.MutateNeuronTraitsProb = 0.0;
    params.MutateLinkTraitsProb = 0.0;
    /// Defines the initial conditions of the genome
    // (?, number of inputs, number if hidden neurons/layers?, number of outputs, ?, output activation function,
    // (hidden activation function, ?, parameters, number of layers)
    NEAT::Genome genome(0,2,3,1, false,NEAT::UNSIGNED_SIGMOID, NEAT::UNSIGNED_SIGMOID, 0,params,0);
    population = std::make_shared<NEAT::Population>(genome, params, true, 1.0, randomNum->getSeed());
    population->Epoch();
    //
    /// Create population of genomes
    std::unique_ptr<GenomeFactory> gf = std::make_unique<GenomeFactory>();
    for (int i = 0; i < settings->populationSize; i++)
    {
        nextGenGenomes.push_back(gf->createGenome(1, randomNum, settings));
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

void EA_MultiNEAT::replacement() {

}

void EA_MultiNEAT::selection() {

}

void EA_MultiNEAT::mutation() {

}

void EA_MultiNEAT::update() {

}

void EA_MultiNEAT::end() {

}

void EA_MultiNEAT::createIndividual(int ind) {

}

void EA_MultiNEAT::loadBestIndividualGenome(int sceneNum) {

}

void EA_MultiNEAT::setFitness(int ind, float fit) {

}

std::shared_ptr<Morphology> EA_MultiNEAT::getMorph() {
    return std::shared_ptr<Morphology>();
}
