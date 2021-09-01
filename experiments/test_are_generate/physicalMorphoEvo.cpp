#include "physicalMorphoEvo.hpp"

using namespace are;

void PMEIndividual::createMorphology(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
    morphology.reset(new sim::Morphology_CPPNMatrix(parameters));
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setGenome(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,0.15);
    nn = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getGenome();
    morphDesc = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getMorphDesc();
    testRes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getRobotManRes();
}


void PhysicalMorphoEvo::init(){
    params = NEAT::Parameters();
    /// Set parameters for NEAT
    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    params.PopulationSize = pop_size;
    params.DynamicCompatibility = true;
    params.CompatTreshold = 2.0;
    params.YoungAgeTreshold = 15;
    params.SpeciesMaxStagnation = 100;
    params.OldAgeTreshold = 50;
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

    // Crossover
    params.SurvivalRate = 0.01;
    params.CrossoverRate = 0.01;
    params.CrossoverRate = 0.01;
    params.InterspeciesCrossoverRate = 0.01;

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

    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProbability").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
}

void PhysicalMorphoEvo::init_next_pop(){
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    // Morphology
    NEAT::Genome morph_genome(0, 5, 10, 6, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 10);
    morph_population = std::make_unique<NEAT::Population>(morph_genome, params, true, 1.0, randomNum->getSeed());
    for (size_t i = 0; i < population_size; i++){ // Body plans
        EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
        CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i)));
        PMEIndividual::Ptr ind(new PMEIndividual(morphgenome,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void PhysicalMorphoEvo::write_data_for_generate(){

    const auto& ind = population[currentIndIndex];
    std::stringstream sst;
    sst << "morph_desc_" << currentIndIndex;
    std::ofstream ofs(Logging::log_folder + std::string("/waiting_to_be_built/")  + sst.str() , std::ios::out | std::ios::ate | std::ios::app);

    if(!ofs)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/waiting_to_be_built/")  + sst.str() << std::endl;
        return;
    }

    for(int j = 0; j < std::dynamic_pointer_cast<PMEIndividual>(ind)->get_morphDesc().cols(); j++)
        ofs << std::dynamic_pointer_cast<PMEIndividual>(ind)->get_morphDesc()(j) << ";";

}
