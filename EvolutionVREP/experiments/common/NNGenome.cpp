#include "NNGenome.hpp"
#include "settings.hpp"

using namespace are;

void NNGenome::init(NEAT::RNG &rng){
    init();
    nn_genome.Randomize_LinkWeights(neat_parameters.MaxWeight,rng);
    nn_genome.Mutate_NeuronBiases(neat_parameters,rng);
}

void NNGenome::init()
{
    std::cout << "init GENOME" << std::endl;
    //Useful parameters for mutation of the weights within MultiNEAT
    neat_parameters.MutateWeightsSevereProb = settings::getParameter<settings::Float>(parameters,"#MutateWeightsSevereProb").value;
    neat_parameters.WeightMutationRate = settings::getParameter<settings::Float>(parameters,"#WeightMutationRate").value;
    neat_parameters.WeightMutationMaxPower = settings::getParameter<settings::Float>(parameters,"#WeightMutationMaxPower").value;
    neat_parameters.WeightReplacementRate = settings::getParameter<settings::Float>(parameters,"#WeightReplacementRate").value;
    neat_parameters.WeightReplacementMaxPower = settings::getParameter<settings::Float>(parameters,"#WeightReplacementMaxPower").value;
    neat_parameters.MaxWeight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nbr_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
    int nbr_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;
    int nbr_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;

    if(nn_type == settings::FFNN)
        nn_genome = NEAT::Genome(0,nbr_input+1,nbr_hidden,nbr_output,
                                 false,NEAT::ActivationFunction::SIGNED_SIGMOID,NEAT::ActivationFunction::SIGNED_SIGMOID,
                                 1,neat_parameters,1);
    else if(nn_type == settings::CTRNN)
        nn_genome = NEAT::Genome(0,nbr_input+1,nbr_hidden,nbr_output,NEAT::ActivationFunction::SIGNED_SIGMOID,NEAT::ActivationFunction::SIGNED_SIGMOID,neat_parameters);



//    for(auto &link_genes : nn_genome.m_LinkGenes){
//        link_genes.SetWeight(randomNum->randFloat(-neat_parameters.MaxWeight,neat_parameters.MaxWeight));
//    }
}

void NNGenome::mutate(NEAT::RNG &rng){
    nn_genome.Mutate_LinkWeights(neat_parameters,rng);
    nn_genome.Mutate_NeuronBiases(neat_parameters,rng);
}

Genome::Ptr NNGenome::crossover(const Genome::Ptr &gen){
    //No Crossover, just simple cloning
    return std::dynamic_pointer_cast<NNGenome>(clone());
}
