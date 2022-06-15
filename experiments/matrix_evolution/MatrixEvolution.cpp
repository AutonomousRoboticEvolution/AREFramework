#include "MatrixEvolution.h"

using namespace are;

void MATRIXEVOLUTION::init()
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

    int operation_mode = settings::getParameter<settings::Integer>(parameters,"#operationMode").value;
    std::string genome_pool = settings::getParameter<settings::String>(parameters,"#genomePool").value;
    if(operation_mode == 0 || operation_mode == 1){ // Simply load or mutation - single robot
        std::string genome_id = settings::getParameter<settings::String>(parameters,"#firstParentID").value;
        std::string temp_string = genome_pool + genome_id;
        first_parent_matrix_4d = protomatrix::load_robot_matrix(temp_string);
    } else if(operation_mode == 2){ // Crossover
        std::string genome_id = settings::getParameter<settings::String>(parameters,"#firstParentID").value;
        std::string temp_string = genome_pool + genome_id;
        first_parent_matrix_4d = protomatrix::load_robot_matrix(temp_string);
        genome_id = settings::getParameter<settings::String>(parameters,"#secondParentID").value;
        temp_string = genome_pool + genome_id;
        second_parent_matrix_4d = protomatrix::load_robot_matrix(temp_string);
    } else if(operation_mode == 3) { // List of robots
        load_list_robot_matrix(genome_pool);
    } else{
        std::cerr << "Operation_mode: " << operation_mode << " " << __func__ << std::endl;
        assert(false);
    }
    initPopulation();

    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProbability").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
}

void MATRIXEVOLUTION::initPopulation()
{
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool cppn_fixed = settings::getParameter<settings::Boolean>(parameters,"#cppnFixedStructure").value;

    rng.Seed(randomNum->getSeed());
    // Morphology
    if(instance_type == settings::INSTANCE_SERVER && simulator_side){
        EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
        NN2CPPNGenome::Ptr morphgenome(new NN2CPPNGenome(randomNum,parameters));
//        child_matrix_4d = first_parent_matrix_4d;
        child_matrix_4d = protomatrix::mutate_matrix(first_parent_matrix_4d);
//        child_matrix_4d = protomatrix::crossover_matrix(first_parent_matrix_4d,second_parent_matrix_4d);
        morphgenome->set_matrix_4d(child_matrix_4d);
        if(cppn_fixed)
            morphgenome->fixed_structure();
        else
            morphgenome->random();
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }else{
        const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        const int evaluations_number = settings::getParameter<settings::Integer>(parameters,"#evaluationsNumber").value;
        for (size_t i = 0; i < population_size*evaluations_number; i++){ // Body plans
            int counter = floor(i / evaluations_number);
            EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
            NN2CPPNGenome::Ptr morphgenome(new NN2CPPNGenome(randomNum,parameters));
            int operation_mode = settings::getParameter<settings::Integer>(parameters,"#operationMode").value;
            if(operation_mode == 0) // No changes - single robot
                child_matrix_4d = first_parent_matrix_4d;
            else if(operation_mode == 1) // Mutation - single robot
                child_matrix_4d = protomatrix::mutate_matrix(first_parent_matrix_4d);
            else if(operation_mode == 2) // Crossover
                child_matrix_4d = protomatrix::crossover_matrix(first_parent_matrix_4d,second_parent_matrix_4d);
            else if(operation_mode == 3) // No changes - set robots
                child_matrix_4d = protomatrix::mutate_matrix(list_parents_matrix_4d.at(counter));
                //child_matrix_4d = list_parents_matrix_4d.at(i);
            else{
                std::cerr << "Operation_mode: " << operation_mode << " " << __func__ << std::endl;
                assert(false);
            }
            morphgenome->set_matrix_4d(child_matrix_4d);
            if(cppn_fixed)
                morphgenome->fixed_structure();
            else
                morphgenome->random();
            CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }
    }

}

void MATRIXEVOLUTION::epoch(){
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    bool only_organ = settings::getParameter<settings::Boolean>(parameters,"#onlyOrganNovelty").value;
    /** NOVELTY **/
    if(Novelty::k_value >= population.size())
        Novelty::k_value = population.size()/2;
    else Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;

    std::vector<Eigen::VectorXd> pop_desc;
    for (size_t i = 0; i < population_size; i++) { // Body plans
        pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(population[i])->getMorphDesc().getCartDesc());
    }
    //compute novelty score
    for (size_t i = 0; i < population_size; i++) { // Body plans
        Eigen::VectorXd ind_desc;
        ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(population[i])->getMorphDesc().getCartDesc();

        if(only_organ)
            ind_desc = ind_desc.block<4,1>(4,0);


        //Compute distances to find the k nearest neighbors of ind
        std::vector<size_t> pop_indexes;
        std::vector<double> distances = Novelty::distances(ind_desc,archive,pop_desc,pop_indexes);

        //Compute novelty
        double ind_nov = Novelty::sparseness(distances);

        //set the objetives
        std::vector<double> objectives = {ind_nov / 2.64 /*+ organ_score)/2.f*/}; /// \todo EB: define 2.64 as constant. This constants applies only for cartesian descriptor!
        std::dynamic_pointer_cast<CPPNIndividual>(population[i])->setObjectives(objectives);


    }
    //update archive for novelty score
    for (size_t i = 0; i < population_size; i++) { // Body plans
        Eigen::VectorXd ind_desc;
        ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(population[i])->getMorphDesc().getCartDesc();

        double ind_nov = std::dynamic_pointer_cast<CPPNIndividual>(population[i])->getObjectives().back();
        Novelty::update_archive(ind_desc,ind_nov,archive,randomNum);
    }

    NSGA2::epoch();

}

void MATRIXEVOLUTION::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}


bool MATRIXEVOLUTION::is_finish()
{
    unsigned int maxGenerations = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;
    return get_generation() > maxGenerations;
}

NEAT::Genome MATRIXEVOLUTION::loadInd(short genomeID)
{
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::cout << "Loading genome: " << genomeID << "!" << std::endl;
    std::stringstream filepath;
    filepath << loadExperiment << "/morphGenome" << genomeID;
    NEAT::Genome indGenome(filepath.str().c_str());
    return indGenome;
}

std::vector<int> MATRIXEVOLUTION::listInds()
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

bool MATRIXEVOLUTION::update(const Environment::Ptr& env)
{
    return true;
}

void MATRIXEVOLUTION::load_list_robot_matrix(std::string filepath)
{
    int robots_number = settings::getParameter<settings::Integer>(parameters,"#robotsNumber").value;
    std::vector<std::vector<double>> robot_matrix_4d;
    for(int i = 0; i < robots_number; i++){
        std::string robot_filepath = filepath + "matrices_" + std::to_string(i);
        robot_matrix_4d.clear();
        robot_matrix_4d.resize(6);
        // Create an input filestream
        std::ifstream myFile(robot_filepath);
        if(!myFile.is_open()) throw std::runtime_error("Could not open file");
        std::string line;
        std::vector<std::string> split_str;
        int output_counter = 0;
        while(std::getline(myFile, line)){
            // Create a stringstream of the current line
            std::stringstream ss(line);
            // Keep track of the current column index
            boost::split(split_str,line,boost::is_any_of(","),boost::token_compress_on); // boost::token_compress_on means it will ignore any empty lines (where there is adjacent newline charaters)
            for(int i = 1; i < split_str.size()-1; i++){
                robot_matrix_4d.at(output_counter).push_back(std::stod(split_str.at(i)));
            }
            output_counter++;
        }
        // Close file
        myFile.close();
        list_parents_matrix_4d.push_back(robot_matrix_4d);
    }
}