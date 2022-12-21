#include "BodyPlanTesting.h"

using namespace are;
int BODYPLANTESTING::novelty_params::k_value = 15;
double BODYPLANTESTING::novelty_params::archive_adding_prob = 0.4;
double BODYPLANTESTING::novelty_params::novelty_thr = 0.9;
void BODYPLANTESTING::init()
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
    params.MutateAddLinkProb = 0.1;
    params.MutateAddNeuronProb = 0.1;
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

    novelty_params::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProbability").value;
    novelty_params::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
}

void BODYPLANTESTING::initPopulation()
{
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    rng.Seed(randomNum->getSeed());
    // Morphology
    NEAT::Genome morph_genome(0, 5, 10, 6, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 10);
    morph_population = std::make_unique<NEAT::Population>(morph_genome, params, true, 1.0, randomNum->getSeed());
    for (size_t i = 0; i < population_size; i++){ // Body plans
        EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
        CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i )));
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);

    }
}

void BODYPLANTESTING::epoch(){
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    bool only_organ = settings::getParameter<settings::Boolean>(parameters,"#onlyOrganNovelty").value;
    int nbr_rep = settings::getParameter<settings::Integer>(parameters,"#nbrRepetitions").value;
    if(repetition < nbr_rep){
        repetition++;
        return;
    }

    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(novelty_params::k_value >= population.size())
            novelty_params::k_value = population.size()/2;
        else novelty_params::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for (size_t i = 0; i < population_size; i++) { // Body plans
            pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(population.at(i))->getMorphDesc());
        }
        //compute novelty score
        for (size_t i = 0; i < population_size; i++) { // Body plans
            Eigen::VectorXd ind_desc;
            ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(population.at(i))->getMorphDesc();
            if(only_organ)
                ind_desc = ind_desc.block<4,1>(4,0);
            //Compute distances to find the k nearest neighbors of ind
            std::vector<size_t> pop_indexes;
            std::vector<double> distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes);

            //Compute novelty
            double ind_nov = novelty::sparseness<novelty_params>(distances);

            //set the objetives
            std::vector<double> objectives = {ind_nov / 2.64}; /// \todo EB: define 2.64 as constant. This constants applies only for cartesian descriptor!
// Body plans
            std::dynamic_pointer_cast<CPPNIndividual>(population.at(i))->setObjectives(objectives);


        }
        //update archive for novelty score
        for (size_t i = 0; i < population_size; i++) { // Body plans
            Eigen::VectorXd ind_desc;
            ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(population.at(i))->getMorphDesc();;

            double ind_nov = std::dynamic_pointer_cast<CPPNIndividual>(population.at(i))->getObjectives().back();
            novelty::update_archive<novelty_params>(ind_desc,ind_nov,archive,randomNum);
        }
    }
    /** MultiNEAT **/
    for (size_t i = 0; i < population_size; i++) {
        morph_population->AccessGenomeByIndex(i).SetFitness(population.at(i)->getObjectives().back());
    }

    morph_population->Epoch();
}

void BODYPLANTESTING::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

void BODYPLANTESTING::init_next_pop(){

    int nbr_rep = settings::getParameter<settings::Integer>(parameters,"#nbrRepetitions").value;
    if(repetition < nbr_rep){
        return;
    }
    repetition = 0;

    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    population.clear();
    rng.Seed(randomNum->getSeed());
    for (size_t i = 0; i < population_size; i++) // Body plans
    {
        EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
        CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i)));
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);

    }
}

bool BODYPLANTESTING::is_finish()
{
    unsigned int maxGenerations = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;
    return get_generation() > maxGenerations;
}

NEAT::Genome BODYPLANTESTING::loadInd(short genomeID)
{
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::cout << "Loading genome: " << genomeID << "!" << std::endl;
    std::stringstream filepath;
    filepath << loadExperiment << "/morphGenome" << genomeID;
    NEAT::Genome indGenome(filepath.str().c_str());
    return indGenome;
}

std::vector<int> BODYPLANTESTING::listInds()
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

bool BODYPLANTESTING::update(const Environment::Ptr& env)
{
    return true;
}
