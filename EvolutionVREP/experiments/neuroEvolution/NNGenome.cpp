#include "NNGenome.hpp"
#include "settings.hpp"

using namespace are;

NNGenome::init()
{
    //Useful parameters for mutation of the weights within MultiNEAT
    neat_parameters.MutateWeightsSevereProb = settings::getParameter<settings::Float>(parameters,"#MutateWeightsSevereProb").value;
    neat_parameters.WeightMutationRate = settings::getParameter<settings::Float>(parameters,"#WeightMutationRate").value;
    neat_parameters.WeightMutationMaxPower = settings::getParameter<settings::Float>(parameters,"#WeightMutationMaxPower").value;
    neat_parameters.WeightReplacementRate = settings::getParameter<settings::Float>(parameters,"#WeightReplacementRate").value;
    neat_parameters.WeightReplacementMaxPower = settings::getParameter<settings::Float>(parameters,"#WeightReplacementMaxPower").value;
    neat_parameters.MaxWeight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;


    if(nn_type == settings::FFNN)
        n_genome = NEAT::Genome(0,nbr_input+1,nbr_hidden,false,nbr_ouput,af_output,af_hidden,neat_parameters);
    else if(nn_type == settings::CTRNN)
        n_genome = NEAT::Genome(0,nbr_input+1,nbr_hidden,nbr_ouput,af_output,af_hidden,neat_parameters);
}

NNGenome::mutate(){
    nn_genome.Mutate_LinkWeights(neat_parameters,rng);
}
