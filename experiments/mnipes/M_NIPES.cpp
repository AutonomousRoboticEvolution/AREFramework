#include "M_NIPES.hpp"

using namespace are;

fitness_fct_t FitnessFunctions::best_fitness = [](const CMAESLearner::Ptr& learner) -> double
{
    return 1 - learner->get_best_solution().first;
};

fitness_fct_t FitnessFunctions::average_fitness = [](const CMAESLearner::Ptr& learner) -> double
{
    auto pop = learner->get_population();
    std::vector<double> fitnesses;
    for(const auto &ind : pop)
        fitnesses.push_back(ind.objectives[0]);

    if(fitnesses.size() > 10){
        std::sort(fitnesses.begin(),fitnesses.end(),std::greater<double>());
    }

    double avg = 0;
    for(int i =0; i< 10; i++){
        avg+=fitnesses[i];
    }
    return avg/10.f;

};

fitness_fct_t FitnessFunctions::learning_progress = [](const CMAESLearner::Ptr& learner) -> double
{
    return learner->learning_progress();
};

void M_NIPESIndividual::createMorphology(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();

    morphology.reset(new CPPNMorph(parameters));
    morphology->set_randNum(randNum);
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<CPPNMorph>(morphology)->setNEATCPPN(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    no_actuation = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_jointNumber() == 0
            && std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_wheelNumber() == 0;
    no_sensors = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_sensorNumber() == 0;


    std::dynamic_pointer_cast<CPPNMorph>(morphology)->createAtPosition(init_x,init_y,init_z);
    float pos[3];
    simGetObjectPosition(std::dynamic_pointer_cast<CPPNMorph>(morphology)->getMainHandle(),-1,pos);
    setGenome();
    setMorphDesc();
    setManRes();
}

void M_NIPESIndividual::createController(){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;

    //If first evaluation on this morphology : init the learner (CMAES)
    if(std::dynamic_pointer_cast<CMAESLearner>(learner)->get_nbr_eval() == 0)
    {
        int wheel_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_wheelNumber();
        int joint_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_jointNumber();
        int sensor_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_sensorNumber();

        nn_inputs = sensor_nbr*2;
        nn_outputs = wheel_nbr + joint_nbr;

        int nbr_weights, nbr_bias;
        if(nn_type == settings::nnType::FFNN)
            NN2Control<ffnn_t>::nbr_parameters(nn_inputs,nb_hidden,nn_outputs,nbr_weights,nbr_bias);
        else if(nn_type == settings::nnType::RNN)
            NN2Control<rnn_t>::nbr_parameters(nn_inputs,nb_hidden,nn_outputs,nbr_weights,nbr_bias);
        else if(nn_type == settings::nnType::ELMAN)
            NN2Control<elman_t>::nbr_parameters(nn_inputs,nb_hidden,nn_outputs,nbr_weights,nbr_bias);
        else {
            std::cerr << "unknown type of neural network" << std::endl;
            return;
        }

        learner.reset(new CMAESLearner(nbr_weights, nbr_bias,nn_inputs,nn_outputs));
        learner->set_parameters(parameters);
        std::dynamic_pointer_cast<CMAESLearner>(learner)->set_randNum(randNum);

        if(use_ctrl_arch){
            auto& starting_gen = controller_archive.archive[wheel_nbr][joint_nbr][sensor_nbr].first;

            if(!use_ctrl_arch || (starting_gen->get_weights().empty() && starting_gen->get_biases().empty()))
                std::dynamic_pointer_cast<CMAESLearner>(learner)->init();
            else{
                std::vector<double> init_pt = std::dynamic_pointer_cast<NNParamGenome>(starting_gen)->get_full_genome();
                std::dynamic_pointer_cast<CMAESLearner>(learner)->init(init_pt);
            }
        }else std::dynamic_pointer_cast<CMAESLearner>(learner)->init();


        std::dynamic_pointer_cast<CMAESLearner>(learner)->next_pop();

    }

    auto nn_params = std::dynamic_pointer_cast<CMAESLearner>(learner)->update_ctrl(control);
    std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->set_weights(nn_params.first);
    std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->set_biases(nn_params.second);
}

void M_NIPESIndividual::update(double delta_time){
    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);

    morphology->command(outputs);
    energy_cost += std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_energy_cost();
    if(std::isnan(energy_cost))
        energy_cost = 0;
    sim_time = delta_time;
}

void M_NIPESIndividual::setGenome()
{
    nn = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getNEATCPPN();
}

void M_NIPESIndividual::setMorphDesc()
{
    morphDesc = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getMorphDesc();
}

void M_NIPESIndividual::setManRes()
{
    testRes = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getRobotManRes();
}



Eigen::VectorXd M_NIPESIndividual::descriptor(){
    if(descriptor_type == FINAL_POSITION){
        double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
        Eigen::VectorXd desc(3);
        desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
        return desc;
    }else if(descriptor_type == VISITED_ZONES){
        Eigen::MatrixXd vz = visited_zones.cast<double>();
        Eigen::VectorXd desc(Eigen::Map<Eigen::VectorXd>(vz.data(),vz.cols()*vz.rows()));
        return desc;
    }
}

std::string M_NIPESIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<M_NIPESIndividual>();
    oarch.register_type<NNParamGenome>();
    oarch.register_type<CPPNGenome>();
    oarch.register_type<CMAESLearner>();
    oarch << *this;
    return sstream.str();
}

void M_NIPESIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<M_NIPESIndividual>();
    iarch.register_type<NNParamGenome>();
    iarch.register_type<CPPNGenome>();
    iarch.register_type<CMAESLearner>();
    iarch >> *this;

    //set parameters and randNum to the genome as it is not contained in the serialisation
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}


void M_NIPES::init(){
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    //Novelty parameters
    Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProb").value;
    bool start_from_exp = settings::getParameter<settings::Boolean>(parameters,"#loadPrevExperiment").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    int fitness_type = settings::getParameter<settings::Integer>(parameters,"#fitnessType").value;

    if(fitness_type == BEST_FIT)
        fitness_fct = FitnessFunctions::best_fitness;
    else if(fitness_type == AVG_FIT)
        fitness_fct = FitnessFunctions::average_fitness;
    else if(fitness_type == LEARNING_PROG)
        fitness_fct = FitnessFunctions::learning_progress;
    else fitness_fct = FitnessFunctions::best_fitness;

    //Initialized the population of morphologies
    if(!simulator_side || instance_type == settings::INSTANCE_REGULAR){
        if(start_from_exp){
            std::string exp_folder = settings::getParameter<settings::String>(parameters,"#startFromExperiment").value;
            generation = findLastGen(exp_folder);
            if(use_ctrl_arch){//Load controller archive from previous experiment
                std::stringstream sstr;
                sstr << generation;
                loadControllerArchive(exp_folder + std::string("/") + "controller_archive"+ sstr.str());
            }
        }else{
            if(use_ctrl_arch){
                int max_nbr_organs = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;
                controller_archive.init(max_nbr_organs,max_nbr_organs,max_nbr_organs);
                bool bootst_ca = settings::getParameter<settings::Boolean>(parameters,"#bootstrapControllerArchive").value;
                if(bootst_ca){//Load controller archive from previous experiment
                    std::string load_archive = settings::getParameter<settings::String>(parameters,"#loadPreviousArchive").value;
                    loadControllerArchive(load_archive);
                    controller_archive.reset_fitnesses();
                }
            }
        }
        init_morph_pop();
        //        std::stringstream sstr;
        //        sstr << "morph_" << morphIDList[morphCounter];
        //        sub_folder = sstr.str();
        //        if(!boost::filesystem::exists(Logging::log_folder + std::string("/") + sub_folder))
        //            boost::filesystem::create_directory(Logging::log_folder + std::string("/") + sub_folder);
    }
    if(instance_type == settings::INSTANCE_SERVER && simulator_side){
        NEAT::Genome mgen(0, 5, 10, 6, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, neat_params, 10);
        CPPNGenome::Ptr morph_gen(new CPPNGenome(mgen));
        morph_gen->set_parameters(parameters);
        morph_gen->set_randNum(randomNum);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        CMAESLearner::Ptr cma_learner(new CMAESLearner);
        Individual::Ptr ind(new M_NIPESIndividual(morph_gen,ctrl_gen,cma_learner));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }

}

void M_NIPES::init_morph_pop(){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool start_from_exp = settings::getParameter<settings::Boolean>(parameters,"#loadPrevExperiment").value;
    bool bootstrap_pop = settings::getParameter<settings::Boolean>(parameters,"#bootstrapPopulation").value;
    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    if(bootstrap_pop){
        //Load the bootstrap population
        listMorphGenomeID(morphIDList);
        pop_size = morphIDList.size();
    }

    /// Set parameters for NEAT
    neat_params.PopulationSize = pop_size;
    neat_params.DynamicCompatibility = true;
    neat_params.CompatTreshold = 2.0;
    neat_params.YoungAgeTreshold = 15;
    neat_params.SpeciesMaxStagnation = 100;
    neat_params.OldAgeTreshold = 50;
    neat_params.MinSpecies = 5;
    neat_params.MaxSpecies = 10;
    neat_params.RouletteWheelSelection = false;

    neat_params.MutateRemLinkProb = 0.02;
    neat_params.RecurrentProb = 0.0;
    neat_params.OverallMutationRate = 0.15;
    neat_params.MutateAddLinkProb = 0.08;
    neat_params.MutateAddNeuronProb = 0.01;


    neat_params.MutateWeightsProb = 0.90;
    neat_params.MaxWeight = 8.0;
    neat_params.WeightMutationMaxPower = 0.2;
    neat_params.WeightReplacementMaxPower = 1.0;

    neat_params.MutateActivationAProb = 0.0;
    neat_params.ActivationAMutationMaxPower = 0.5;
    neat_params.MinActivationA = 0.05;
    neat_params.MaxActivationA = 6.0;

    neat_params.MutateNeuronActivationTypeProb = 0.03;

    // Crossover
    neat_params.SurvivalRate = 0.01;
    neat_params.CrossoverRate = 0.01;
    neat_params.CrossoverRate = 0.01;
    neat_params.InterspeciesCrossoverRate = 0.01;

    neat_params.ActivationFunction_SignedSigmoid_Prob = 0.0;
    neat_params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
    neat_params.ActivationFunction_Tanh_Prob = 1.0;
    neat_params.ActivationFunction_TanhCubic_Prob = 0.0;
    neat_params.ActivationFunction_SignedStep_Prob = 1.0;
    neat_params.ActivationFunction_UnsignedStep_Prob = 0.0;
    neat_params.ActivationFunction_SignedGauss_Prob = 1.0;
    neat_params.ActivationFunction_UnsignedGauss_Prob = 0.0;
    neat_params.ActivationFunction_Abs_Prob = 0.0;
    neat_params.ActivationFunction_SignedSine_Prob = 1.0;
    neat_params.ActivationFunction_UnsignedSine_Prob = 0.0;
    neat_params.ActivationFunction_Linear_Prob = 1.0;

    NEAT::Genome morph_gen(0, 5, 10, 6, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, neat_params, 10);
    morph_population.reset(new NEAT::Population(morph_gen,neat_params,true,1.0,randomNum->getSeed()));

    std::string exp_folder = settings::getParameter<settings::String>(parameters,"#startFromExperiment").value;



    if(bootstrap_pop || start_from_exp){
        for(unsigned i = 0; i < pop_size; i++){
            if(start_from_exp){
                std::stringstream sstr;
                sstr << generation << "_" << i;
                if(verbose)
                    std::cout << "Load morphology genome : " << exp_folder + std::string("/") + "morphGenome" + sstr.str() << std::endl;
                morph_gen = NEAT::Genome((exp_folder + std::string("/") + "morphGenome" + sstr.str()).c_str());
            }else if(bootstrap_pop)
                loadNEATGenome(morphIDList[i],morph_gen);
            morph_population->AccessGenomeByIndex(i) = morph_gen;
        }
    }
    for(unsigned i = 0; i < pop_size ; i++){
        NEAT::Genome mgen = morph_population->AccessGenomeByIndex(i);
        CPPNGenome::Ptr morph_gen(new CPPNGenome(mgen));
        morph_gen->set_parameters(parameters);
        morph_gen->set_randNum(randomNum);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        CMAESLearner::Ptr cma_learner(new CMAESLearner);
        Individual::Ptr ind(new M_NIPESIndividual(morph_gen,ctrl_gen,cma_learner));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_ctrl_archive(controller_archive);
        population.push_back(ind);
    }
}

void M_NIPES::epoch(){

    int max_organs = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    
    //update controller archive
    if(use_ctrl_arch){
        std::vector<double> weights;
        std::vector<double> biases;
        int nbr_weights;
        NNParamGenome best_gen;
        for(const auto& ind: population){
            //Pick up the best controller found by the learner
            auto &best_controller = std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner())->get_best_solution();
            if(best_controller.second.empty())
                continue;
            nbr_weights = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_weights().size();
            weights.clear();
            biases.clear();
            weights.insert(weights.begin(),best_controller.second.begin(),best_controller.second.begin()+nbr_weights);
            biases.insert(biases.begin(),best_controller.second.begin()+nbr_weights,best_controller.second.end());
            best_gen.set_weights(weights);
            best_gen.set_biases(biases);
            std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_ctrl_genome(std::make_shared<NNParamGenome>(best_gen)); //put best genome back in the ind for log
            //update the archive
            const Eigen::VectorXd &morph_desc = std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->getMorphDesc();
            controller_archive.update(std::make_shared<NNParamGenome>(best_gen),1-best_controller.first,morph_desc[4]*max_organs,morph_desc[6]*max_organs,morph_desc[5]*max_organs);
        }
    }
    //Epoch the morphogenesis
    for(unsigned i = 0; i < population.size(); i++)
        morph_population->AccessGenomeByIndex(i).SetFitness(population[i]->getObjectives()[0]);
    morph_population->Epoch();
}


void M_NIPES::init_next_pop(){
    population.clear();
    for(int i = 0; i < morph_population->NumGenomes() ; i++){
        NEAT::Genome mgen = morph_population->AccessGenomeByIndex(i);
        CPPNGenome::Ptr morph_gen(new CPPNGenome(mgen));
        morph_gen->set_parameters(parameters);
        morph_gen->set_randNum(randomNum);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        CMAESLearner::Ptr cma_learner(new CMAESLearner);
        Individual::Ptr ind(new M_NIPESIndividual(morph_gen,ctrl_gen,cma_learner));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_ctrl_archive(controller_archive);
        population.push_back(ind);
    }
}

bool M_NIPES::update_maze(const Environment::Ptr &env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    if(simulator_side){
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_final_position(env->get_final_position());
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_trajectory(env->get_trajectory());
        std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner())->update_pop_info(
                    ind->getObjectives(),
                    std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->descriptor()
                    );

        //LEARNING WITH NIP-ES
        if(!std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner())->step())
            return false;
        else{
            auto obj = ind->getObjectives();
            obj[0] = fitness_fct(std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner()));
            ind->setObjectives(obj);
            return true;
        }
    }else return true;
}

bool M_NIPES::update_obstacle_avoidance(const Environment::Ptr &env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    if(simulator_side){
        if(!std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->is_actuated()){
            auto obj = ind->getObjectives();
            obj[0] = 0;
            ind->setObjectives(obj);
            return true;
        }

        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_final_position(env->get_final_position());
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_trajectory(env->get_trajectory());
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_visited_zones(std::dynamic_pointer_cast<sim::ObstacleAvoidance>(env)->get_visited_zone_matrix());
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_descriptor_type(VISITED_ZONES);

        //LEARNING WITH NIP-ES
        std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner())->update_pop_info(
                    ind->getObjectives(),
                    std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->descriptor()
                    );
        learning_finished = std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner())->step();

        if(learning_finished){
            auto obj = ind->getObjectives();
            obj[0] = fitness_fct(std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner()));
            ind->setObjectives(obj);
            learning_finished = false;
            return true;
        }else return false;
    }
    return true;
}

bool M_NIPES::update(const Environment::Ptr& env){
    if(simulator_side){
        if(env->get_name() == "mazeEnv")
            return update_maze(env);
        else if(env->get_name() == "obstacle_avoidance")
            return update_obstacle_avoidance(env);
    }
}

//bool M_NIPES::is_finish(){
//    int maxNbrEval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;

//    return (numberEvaluation >= maxNbrEval || _is_finish || );
//}

void M_NIPES::loadNEATGenome(short int genomeID, NEAT::Genome &gen){
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::cout << "Loading genome: " << genomeID << "!" << std::endl;
    std::stringstream filepath;
    filepath << loadExperiment << "/morphGenome" << genomeID;
    gen = NEAT::Genome(filepath.str().c_str());
}


void M_NIPES::listMorphGenomeID(std::vector<short int>& list){
    // This code snippet was taken from: https://www.gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::string bootstrapFile = settings::getParameter<settings::String>(parameters,"#bootstrapFile").value;
    // Create an input filestream
    std::ifstream myFile(loadExperiment + std::string("/") + bootstrapFile);
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
            list.push_back(val);
            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();
            // Increment the column index
            colIdx++;
        }
    }
    // Close file
    myFile.close();
}

void M_NIPES::loadNbrSenAct(const std::vector<short> &list, std::map<short, morph_desc_t> &desc_map){

    std::string exp_folder = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::string morph_desc_file = settings::getParameter<settings::String>(parameters,"#morphDescFile").value;
    int maxNbrOrgans = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;

    std::ifstream ifs(exp_folder + std::string("/") + morph_desc_file);
    if(!ifs){
        std::cerr << "Unable to open " << exp_folder + std::string("/") + morph_desc_file << std::endl;
        return;
    }
    std::map<short, morph_desc_t> full_desc_map;
    std::vector<std::string> split_line;
    for(std::string line; std::getline(ifs,line);){
        boost::split(split_line,line,boost::is_any_of(","));
        int id = std::stoi(split_line[0]);
        morph_desc_t md;
        md.wheels = static_cast<int>(std::stod(split_line[5])*maxNbrOrgans);
        md.joints = static_cast<int>(std::stod(split_line[7])*maxNbrOrgans);
        md.sensors = static_cast<int>(std::stod(split_line[6])*maxNbrOrgans);
        full_desc_map.emplace(id,md);
    }

    for(const int &id : list)
        desc_map.emplace(id,full_desc_map[id]);
}

void M_NIPES::loadControllerArchive(const std::string &file){
    std::ifstream stream(file);
    if(!stream)
    {
        std::cerr << "unable to open : " << file << std::endl;
        return;
    }

    int maxNbrOrgans = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;

    ControllerArchive archive;
    archive.init(maxNbrOrgans,maxNbrOrgans,maxNbrOrgans);

    std::string elt;
    std::string wheel_str,joint_str,sensor_str;
    int w,j,s;
    int nbr_weights,nbr_biases;
    int state = 0;
    double fitness;
    are::NNParamGenome gen;
    while(std::getline(stream,elt)){
        if(state == 0){
            std::stringstream sstr(elt);
            std::getline(sstr,wheel_str,',');
            w = std::stoi(wheel_str);
            std::getline(sstr,joint_str,',');
            j = std::stoi(joint_str);
            std::getline(sstr,sensor_str,',');
            s = std::stoi(sensor_str);
            state = 1;
        }else if(state == 1){
            std::stringstream sstr;
            sstr << elt << std::endl;
            nbr_weights = std::stoi(elt);
            std::getline(stream,elt);
            sstr << elt << std::endl;
            nbr_biases = std::stoi(elt);
            for(int i = 0; i < nbr_weights; i++){
                std::getline(stream,elt);
                sstr << elt << std::endl;
            }
            for(int i = 0; i < nbr_biases; i++){
                std::getline(stream,elt);
                sstr << elt << std::endl;
            }
            gen.from_string(sstr.str());
            state = 2;
        }else if (state == 2){
            fitness = std::stod(elt);
            archive.archive[w][j][s] = std::make_pair(std::make_shared<are::NNParamGenome>(gen),fitness);
            state = 0;
        }
    }

    controller_archive.archive = archive.archive;
}

int M_NIPES::findLastGen(const std::string &exp_folder){
    std::string fitness_file = settings::getParameter<settings::String>(parameters,"#fitnessFile").value;
    std::ifstream ifs(exp_folder + std::string("/") + fitness_file);
    if(!ifs){
        std::cerr << "unable to open " << exp_folder + std::string("/") + fitness_file << std::endl;
        return 0;
    }

    std::string line;
    std::vector<std::string> lines;
    while(std::getline(ifs,line))
        lines.push_back(line);
    std::stringstream sstr(lines.back());
    std::string gen_str;
    std::getline(sstr,gen_str,',');
    return std::stoi(gen_str);
}

bool M_NIPES::finish_eval(const Environment::Ptr &env){
    int handle = std::dynamic_pointer_cast<CPPNMorph>(population[currentIndIndex]->get_morphology())->getMainHandle();
    float pos[3];
    simGetObjectPosition(handle,-1,pos);

    if(simGetSimulationTime() < 0.1){
        current_ind_past_pos[0] = pos[0];
        current_ind_past_pos[1] = pos[1];
        current_ind_past_pos[2] = pos[2];
        move_counter = 0;
    }else{
        if(fabs(current_ind_past_pos[0] - pos[0]) > 1e-3 ||
                fabs(current_ind_past_pos[1] - pos[1]) > 1e-3 ||
                fabs(current_ind_past_pos[2] - pos[2]) > 1e-3)
            move_counter++;

        current_ind_past_pos[0] = pos[0];
        current_ind_past_pos[1] = pos[1];
        current_ind_past_pos[2] = pos[2];
    }

    bool drop_eval = simGetSimulationTime() > 10.0 && move_counter <= 10;
    if(drop_eval) nbr_dropped_eval++;

    if(env->get_name() == "mazeEnv")
    {
        float tPos[3];
        tPos[0] = settings::getParameter<settings::Double>(parameters,"#target_x").value;
        tPos[1] = settings::getParameter<settings::Double>(parameters,"#target_y").value;
        tPos[2] = settings::getParameter<settings::Double>(parameters,"#target_z").value;
        double fTarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
        double arenaSize = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;

        auto distance = [](float* a,float* b) -> double
        {
            return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                    (a[1] - b[1])*(a[1] - b[1]) +
                    (a[2] - b[2])*(a[2] - b[2]));
        };

        double dist = distance(pos,tPos)/sqrt(2*arenaSize*arenaSize);

        bool stop = dist < fTarget || drop_eval;

        if(stop){
            std::cout << "STOP !" << std::endl;
        }

        return  stop;
    }else if(env->get_name() == "obstacle_avoidance"){
        return drop_eval;
    }
}
