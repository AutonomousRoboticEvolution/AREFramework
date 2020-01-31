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
    NEAT::Genome neat_genome(0, 5, 5, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 0);
    neat_population = std::make_unique<NEAT::Population>(neat_genome, params, true, 1.0, randomNum->getSeed());


    for (size_t i = 0; i < params.PopulationSize ; i++)
    {
        CPPNGenome::Ptr ctrlgenome(new CPPNGenome(neat_population->AccessGenomeByIndex(i)));
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        BOLearner::Ptr learner(new BOLearner);

        learner->set_parameters(parameters);

        EPuckMorphology morph(parameters);
        morph.initSubstrate();
        NEAT::NeuralNetwork nn;
        NEAT::Substrate subs =  morph.get_substrate();
        NEAT::Genome gen = ctrlgenome->get_neat_genome();
        gen.BuildHyperNEATPhenotype(nn,subs);
        learner->init_model(nn.m_connections.size());

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



bool EA_HyperNEAT::update(const Environment::Ptr &env)
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int nbr_bo_iter = settings::getParameter<settings::Integer>(parameters,"#numberBOIteration").value;
    bool globalBOData = settings::getParameter<settings::Boolean>(parameters,"#globalBOData").value;
    float BODataRatio = settings::getParameter<settings::Float>(parameters,"#BODataRatio").value;

    if(generation > 0 && currentFitnesses.size() == 1){
        partialObs.clear();
        partialSpl.clear();
        for(int i = (generation-1)*population.size(); i < observations.size(); i++)
        {
            if(randomNum->randFloat(0,1) < BODataRatio){
                partialObs.push_back(observations[i]);
                partialSpl.push_back(samples[i]);
            }
        }
    }

    Individual::Ptr ind = population[currentIndIndex];

    //add last fitness and NN weights in the database for the Bayesian optimizer
    Eigen::VectorXd o(1);
    o(0) = currentFitnesses.back();
    partialObs.push_back(o);

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
    partialSpl.push_back(s);

    if(verbose)
        std::cout << "Size of dataset for BO : " <<  partialObs.size() << std::endl;

    if(currentFitnesses.size() == nbr_bo_iter || generation == 0)
    {
        if(generation > 0){
            std::dynamic_pointer_cast<CPPNIndividual>(ind)->best_ctrl();
            o(0) = std::dynamic_pointer_cast<BOLearner>(population[currentIndIndex]->get_learner())->get_best_fitness();
            observations.push_back(o);
            samples.push_back(std::dynamic_pointer_cast<BOLearner>(population[currentIndIndex]->get_learner())->get_best_sample());
        }
        else {
            observations.push_back(o);
            samples.push_back(s);
        }
        ind->setFitness(computeFitness());

        if(env->get_name() == "mazeEnv")
            std::dynamic_pointer_cast<CPPNIndividual>(ind)->set_final_position(
                        std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
        else if(env->get_name() == "testEnv")
            std::dynamic_pointer_cast<CPPNIndividual>(ind)->set_final_position(
                        std::dynamic_pointer_cast<TestEnv>(env)->get_final_position());
        else std::cerr << "unknown environment" << std::endl;


        currentFitnesses.clear();
        ind.reset();
        if(!globalBOData){
            observations.clear();
            samples.clear();
        }
        return true;
    }

    if(currentFitnesses.size() == 1){
        std::dynamic_pointer_cast<CPPNIndividual>(ind)->compute_model(partialObs,partialSpl);
    }
    std::dynamic_pointer_cast<CPPNIndividual>(ind)->update_learner(partialObs, partialSpl);

    ind.reset();
    return false;
}

void EA_HyperNEAT::setFitness(size_t indIndex, float fitness){
    currentIndIndex = indIndex;
    currentFitnesses.push_back(fitness);
}

///@todo add novelty
float EA_HyperNEAT::computeFitness(){
    float fit;
    float best_fit;
    if(generation == 0)
        best_fit = currentFitnesses.back();
    else    
    	best_fit = std::dynamic_pointer_cast<BOLearner>(population[currentIndIndex]->get_learner())->get_best_fitness();
    fit = best_fit + (best_fit - currentFitnesses.front());
    return fit;
}

///@todo change how the learner it passed through the generations. Have to modify MultiNeat Epoch
void EA_HyperNEAT::epoch(){
    bool reinit_learner = settings::getParameter<settings::Boolean>(parameters,"#reinitLearner").value;

    std::vector<BOLearner::Ptr> learners;
    for(int i = 0; i < population.size(); i++){
        neat_population->AccessGenomeByIndex(i).SetFitness(population[i]->getFitness());
        if(!reinit_learner){
            BOLearner::Ptr l = std::dynamic_pointer_cast<BOLearner>(std::dynamic_pointer_cast<CPPNIndividual>(population[i])->getLearner());
            learners.push_back(l);
            l.reset();
        }
    }
    neat_population->Epoch();
    population.clear();
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    for (int i = 0; i < pop_size ; i++)
    {
        CPPNGenome::Ptr ctrlgenome(new CPPNGenome(neat_population->AccessGenomeByIndex(i)));
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        BOLearner::Ptr learner;
        if(reinit_learner){
            learner.reset(new BOLearner);
            learner->set_parameters(parameters);
        }
        else learner = learners[i];

        CPPNIndividual::Ptr ind(new CPPNIndividual(no_gen,ctrlgenome,learner));
        ind->set_parameters(parameters);

        population.push_back(ind);

        ctrlgenome.reset();
        no_gen.reset();
    }
}


