#include "BodyPlanTesting.h"

using namespace are;

void BODYPLANTESTING::init()
{
    nb_obj = 1;
    max_obj = {1};
    min_obj = {0};
    nn2::rgen_t::gen.seed(randomNum->getSeed());

    cppn_params::cppn::_mutation_type = settings::getParameter<settings::Integer>(parameters,"#mutationType").value;
    cppn_params::cppn::_mutate_connections = settings::getParameter<settings::Boolean>(parameters,"#mutateConnection").value;
    cppn_params::cppn::_mutate_neurons = settings::getParameter<settings::Boolean>(parameters,"#mutateNeurons").value;;;
    cppn_params::cppn::_max_nb_conns = settings::getParameter<settings::Integer>(parameters,"#maxNbrConnections").value;
    cppn_params::cppn::_min_nb_conns = settings::getParameter<settings::Integer>(parameters,"#minNbrConnections").value;
    cppn_params::cppn::_max_nb_neurons = settings::getParameter<settings::Integer>(parameters,"#maxNbrNeurons").value;
    cppn_params::cppn::_min_nb_neurons = settings::getParameter<settings::Integer>(parameters,"#minNbrNeurons").value;
    cppn_params::cppn::_mutation_rate = settings::getParameter<settings::Float>(parameters,"#rateMutation").value;
    cppn_params::cppn::_rate_mutate_conn = settings::getParameter<settings::Float>(parameters,"#rateMutateConnection").value;
    cppn_params::cppn::_rate_mutate_neur = settings::getParameter<settings::Float>(parameters,"#rateMutateNeuron").value;
    cppn_params::cppn::_rate_add_conn = settings::getParameter<settings::Float>(parameters,"#rateAddConnection").value;
    cppn_params::cppn::_rate_del_conn = settings::getParameter<settings::Float>(parameters,"#rateDeleteConnection").value;
    cppn_params::cppn::_rate_change_conn = settings::getParameter<settings::Float>(parameters,"#rateChangeConnection").value;
    cppn_params::cppn::_rate_add_neuron = settings::getParameter<settings::Float>(parameters,"#rateAddNeuron").value;
    cppn_params::cppn::_rate_del_neuron = settings::getParameter<settings::Float>(parameters,"#rateDeleteNeuron").value;
    cppn_params::cppn::_rate_crossover = settings::getParameter<settings::Float>(parameters,"#rateCrossover").value;
    cppn_params::evo_float::mutation_rate = settings::getParameter<settings::Float>(parameters,"#CPPNParametersMutationRate").value;

    initPopulation();

    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProbability").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
}

void BODYPLANTESTING::initPopulation()
{
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    rng.Seed(randomNum->getSeed());
    // Morphology
    if(instance_type == settings::INSTANCE_SERVER && simulator_side){
        EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
        ProtomatrixGenome::Ptr morphgenome(new ProtomatrixGenome(randomNum,parameters));
        morphgenome->random();
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }else{
        const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        for (size_t i = 0; i < population_size; i++){ // Body plans
            EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
            ProtomatrixGenome::Ptr morphgenome(new ProtomatrixGenome(randomNum,parameters));
            if(settings::getParameter<settings::Boolean>(parameters,"#isRandomStartingPopulation").value)
                morphgenome->random();
            else
                morphgenome->init();

            CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }
    }

}

void BODYPLANTESTING::init_next_pop(){
    int nbr_rep = settings::getParameter<settings::Integer>(parameters,"#nbrRepetitions").value;
    if(repetition < nbr_rep){
        repetition++;
        return;
    }
    repetition = 0;
    NSGA2::init_next_pop();
}

void BODYPLANTESTING::epoch(){
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    int nbr_rep = settings::getParameter<settings::Integer>(parameters,"#nbrRepetitions").value;
    int descriptor = settings::getParameter<settings::Integer>(parameters,"#descriptor").value;

    if(repetition < nbr_rep){
        std::cout << "number of repetitions : " << repetition << " over " << nbr_rep << std::endl;
        return;
    }

    /** NOVELTY **/
    if(Novelty::k_value >= population.size())
        Novelty::k_value = population.size()/2;
    else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

    std::vector<Eigen::VectorXd> pop_desc;
    for (size_t i = 0; i < population_size; i++) { // Body plans
        pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(population[i])->descriptor());
    }
    //compute novelty score
    for (size_t i = 0; i < population_size; i++) { // Body plans
        Eigen::VectorXd ind_desc;
        ind_desc = population[i]->descriptor();


        //Compute distances to find the k nearest neighbors of ind
        std::vector<size_t> pop_indexes;

        std::vector<double> distances;
        if(descriptor == CART_DESC)
            distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes,Novelty::distance_fcts::euclidian);
        else if(descriptor == ORGAN_POSITION)
            distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes,Novelty::distance_fcts::positional);

        //Compute novelty
        double ind_nov = Novelty::sparseness(distances);
        if(descriptor == CART_DESC)
            ind_nov /= 2.64;
        //set the objetives

        std::vector<double> objectives = {ind_nov}; /// \todo EB: define 2.64 as constant. This constants applies only for cartesian descriptor!
        std::dynamic_pointer_cast<CPPNIndividual>(population[i])->setObjectives(objectives);


    }
    //update archive for novelty score
    for (size_t i = 0; i < population_size; i++) { // Body plans
        Eigen::VectorXd ind_desc;
        ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(population[i])->descriptor();;

        double ind_nov = std::dynamic_pointer_cast<CPPNIndividual>(population[i])->getObjectives().back();
        Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
    }

    NSGA2::epoch();

}

void BODYPLANTESTING::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
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
