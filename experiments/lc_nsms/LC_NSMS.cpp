#include "LC_NSMS.h"

using namespace are;

void LC_NSMS::init()
{
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

    initPopulation();

    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProbability").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
}

void LC_NSMS::initPopulation()
{
    rng.Seed(randomNum->getSeed());

    // Morphology
    NEAT::Genome morph_genome(0, 5, 10, 6, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 10);
    morph_population = std::make_unique<NEAT::Population>(morph_genome, params, true, 1.0, randomNum->getSeed());

    int manufacturabilityMethod = settings::getParameter<settings::Integer>(parameters,"#manufacturabilityMethod").value;

    bool isBootstrapPopulation = settings::getParameter<settings::Boolean>(parameters,"#isBootstrapEvolution").value;
    std::vector<int> robotList;
    if(isBootstrapPopulation){
        robotList = listInds();
    }

    for (size_t i = 0; i < params.PopulationSize ; i++)
    {
        if(isBootstrapPopulation) {
            NEAT::Genome indGenome;
            indGenome = loadInd(robotList[i]);
            morph_population->AccessGenomeByIndex(i) = indGenome;
        }
        // Generate random topologies
        /// \todo EB: This if statement might not be necessay here.
        if(manufacturabilityMethod < 0){ // Generate random robots
            NEAT::RNG rng;
            const misc::RandNum::Ptr rn = get_randomNum();
            rng.Seed(rn->randInt(1,100000));
            morph_population->AccessGenomeByIndex(i).Randomize_LinkWeights(params.MaxWeight,rng);
            morph_population->AccessGenomeByIndex(i).Mutate_NeuronBiases(params,rng);

        }
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i)));
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,no_gen));

        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void LC_NSMS::epoch(){
    double linearCombAlpha = settings::getParameter<settings::Double>(parameters,"#linearCombAlpha").value;
    bool isSymDesc = settings::getParameter<settings::Boolean>(parameters,"#isSymDesc").value;
    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(Novelty::k_value >= population.size())
            Novelty::k_value = population.size()/2;
        else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for(const auto& ind : population) {
            if(isSymDesc)
                pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(ind)->getSymDesc());
            else
                pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(ind)->getMorphDesc());
        }
        //compute novelty score
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc;
            if(isSymDesc)
                ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->getSymDesc();
            else
                ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->getMorphDesc();
            //Compute distances to find the k nearest neighbors of ind
            std::vector<size_t> pop_indexes;
            std::vector<double> distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes);

            //Compute novelty
            double ind_nov = Novelty::sparseness(distances);

            /** Linear Combination **/
            double manScore = std::dynamic_pointer_cast<CPPNIndividual>(ind)->getManScore();

            //set the objetives
            std::vector<double> objectives = {linearCombAlpha * (ind_nov / 2.64)  + (1-linearCombAlpha) * manScore}; /// \todo EB: define 2.64 as constant. This constants applies only for cartesian descriptor!
            ind->setObjectives(objectives);
        }

        //update archive for novelty score
        for(const auto& ind : population){
            Eigen::VectorXd ind_desc;
            if(isSymDesc)
                ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->getSymDesc();
            else
                ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->getMorphDesc();

            double ind_nov = ind->getObjectives().back();
            Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
        }
    }
    /** MultiNEAT **/
    int manufacturabilityMethod = settings::getParameter<settings::Integer>(parameters,"#manufacturabilityMethod").value;
    int indCounter = 0; /// \todo EB: There must be a better way to do this!
    for(const auto& ind : population){
        morph_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
        indCounter++;
    }

    if(manufacturabilityMethod >= 0) { // Don't generate random robots
        morph_population->Epoch();
    }
}

void LC_NSMS::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

void LC_NSMS::init_next_pop(){
    int manufacturabilityMethod = settings::getParameter<settings::Integer>(parameters,"#manufacturabilityMethod").value;
    population.clear();
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    for (int i = 0; i < pop_size ; i++)
    {
        if(manufacturabilityMethod < 0) { // Generate random robots
            NEAT::RNG rng;
            const misc::RandNum::Ptr rn = get_randomNum();
            rng.Seed(rn->randInt(1, 10000000));
            morph_population->AccessGenomeByIndex(i).Randomize_LinkWeights(params.MaxWeight, rng);
            morph_population->AccessGenomeByIndex(i).Mutate_NeuronBiases(params, rng);
        }

        CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i)));
        EmptyGenome::Ptr no_gen(new EmptyGenome);
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,no_gen));
        ind->set_parameters(parameters);
        population.push_back(ind);
    }
}

bool LC_NSMS::is_finish()
{
    unsigned int maxGenerations = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;
    return get_generation() > maxGenerations;
}

NEAT::Genome LC_NSMS::loadInd(short genomeID)
{
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::cout << "Loading genome: " << genomeID << "!" << std::endl;
    std::stringstream filepath;
    filepath << loadExperiment << "/morphGenome" << genomeID;
    NEAT::Genome indGenome(filepath.str().c_str());
    return indGenome;
}

std::vector<int> LC_NSMS::listInds()
{
    std::vector<int> robotList;
    // This code snippet was taken from: https://www.gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::string bootstrapFile = settings::getParameter<settings::String>(parameters,"#bootstrapFile").value;
    // Create an input filestream
    std::ifstream myFile(loadExperiment+bootstrapFile);
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");
    std::string line;
    int val;

    while(std::getline(myFile, line)){
        // Create a stringstream of the current line
        std::stringstream ss(line);
        // Keep track of the current column index
        int colIdx = 0;
        // Extract each integer
        while(ss >> val) {
            // Add the current integer to the 'colIdx' column's values vector
            robotList.push_back(val);
            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();
            // Increment the column index
            colIdx++;
        }
    }
    // Close file
    myFile.close();
    return robotList;
}
