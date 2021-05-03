#include "BodyPlanTesting.h"

using namespace are;

void BODYPLANTESTING::init()
{
    nb_obj = 1;
    max_obj = {1};
    min_obj = {0};

    initPopulation();

    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProbability").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
}

void BODYPLANTESTING::initPopulation()
{
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    rng.Seed(randomNum->getSeed());
    // Morphology
    for (size_t i = 0; i < population_size; i++){ // Body plans
        EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
        NN2CPPNGenome::Ptr morphgenome(new NN2CPPNGenome(randomNum,parameters));
        morphgenome->init();
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void BODYPLANTESTING::epoch(){
    const int num_eval = settings::getParameter<settings::Integer>(parameters,"#numberEvaluations").value;
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    /** NOVELTY **/
    if(settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value > 0.){
        if(Novelty::k_value >= population.size())
            Novelty::k_value = population.size()/2;
        else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

        std::vector<Eigen::VectorXd> pop_desc;
        for (size_t i = 0; i < population_size; i++) { // Body plans
            pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(population.at(i * num_eval))->getMorphDesc());
        }
        //compute novelty score
        for (size_t i = 0; i < population_size; i++) { // Body plans
            Eigen::VectorXd ind_desc;
            ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(population.at(i * num_eval))->getMorphDesc();
            //Compute distances to find the k nearest neighbors of ind
            std::vector<size_t> pop_indexes;
            std::vector<double> distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes);

            //Compute novelty
            double ind_nov = Novelty::sparseness(distances);

            //set the objetives
            std::vector<double> objectives = {ind_nov / 2.64}; /// \todo EB: define 2.64 as constant. This constants applies only for cartesian descriptor!
            for (size_t j = 0; j < num_eval; j++) { // Body plans
                std::dynamic_pointer_cast<CPPNIndividual>(population.at(i * num_eval + j))->setObjectives(objectives);
            }

        }
        //update archive for novelty score
        for (size_t i = 0; i < population_size; i++) { // Body plans
            Eigen::VectorXd ind_desc;
            ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(population.at(i * num_eval))->getMorphDesc();;

            double ind_nov = std::dynamic_pointer_cast<CPPNIndividual>(population.at(i * num_eval))->getObjectives().back();
            Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
        }
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
