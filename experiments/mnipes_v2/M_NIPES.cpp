#include "M_NIPES.hpp"

using namespace are;
namespace fs = boost::filesystem;

fitness_fct_t FitnessFunctions::best_fitness = [](const CMAESLearner& learner) -> double
{
    return learner.get_best_solution().objectives[0];
};

fitness_fct_t FitnessFunctions::average_fitness = [](const CMAESLearner& learner) -> double
{
    auto pop = learner.get_population();
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

fitness_fct_t FitnessFunctions::learning_progress = [](const CMAESLearner& learner) -> double
{
    return learner.learning_progress();
};

selection_fct_t SelectionFunctions::best_of_subset = [](const std::vector<genome_t>& gene_pool) -> NN2CPPNGenome
{
    int obj_idx = settings::getParameter<settings::Integer>(gene_pool[0].morph_genome.get_parameters(),"#morphSelectionObjective").value;

    double best_fitness = gene_pool[0].objectives[obj_idx];
    int best_idx = 0;
    for(int i = 1; i < gene_pool.size(); i++){
        if(best_fitness < gene_pool[i].objectives[obj_idx]){
            best_fitness = gene_pool[i].objectives[obj_idx];
            best_idx = i;
        }
    }
    nn2_cppn_t cppn = gene_pool[best_idx].morph_genome.get_cppn();
    cppn.mutate();
    NN2CPPNGenome new_gene(cppn);
    new_gene.set_parents_ids({gene_pool[best_idx].morph_genome.id(),-1});
    return new_gene;
};

selection_fct_t SelectionFunctions::two_best_of_subset = [](const std::vector<genome_t>& gene_pool) -> NN2CPPNGenome
{
    int obj_idx = settings::getParameter<settings::Integer>(gene_pool[0].morph_genome.get_parameters(),"#morphSelectionObjective").value;

    double best_fitness[2] = {gene_pool[0].objectives[obj_idx],gene_pool[1].objectives[obj_idx]};
    int best_idx[2] = {0,1};
    for(int i = 2; i < gene_pool.size(); i++){
        if(best_fitness[0] < gene_pool[i].objectives[obj_idx]){//Best
            best_fitness[0] = gene_pool[i].objectives[obj_idx];
            best_idx[0] = i;
        }else if(best_fitness[1] < gene_pool[i].objectives[obj_idx]){//Second Best
            best_fitness[1] = gene_pool[i].objectives[obj_idx];
            best_idx[1] = i;
        }
    }

    nn2_cppn_t parents[2] = {gene_pool[best_idx[0]].morph_genome.get_cppn(),
                          gene_pool[best_idx[1]].morph_genome.get_cppn()};
    nn2_cppn_t child;
    parents[0].crossover(parents[1],child); //crossover to generate the child
    child.mutate(); //mutate the child
    NN2CPPNGenome new_gene(child);
    new_gene.set_parents_ids({gene_pool[best_idx[0]].morph_genome.id(),gene_pool[best_idx[1]].morph_genome.id()}); //store the ids of the parents
    return new_gene;
};




void M_NIPESIndividual::createMorphology(){
    individual_id = morphGenome->id();
    morphology = std::make_shared<sim::Morphology_CPPNMatrix>(parameters);
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
    int i = rewards.size();

    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->set_morph_id(std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->id());
    if(ctrlGenome->get_type() != "empty_genome")
        std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setLoadRobot();
    else
        std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setDecodeRobot();

    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_position[i*3],init_position[i*3+1],init_position[i*3+2]);
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_cart_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_organ_position_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosDesc());
    setMorphDesc();
    setManRes();
}

void M_NIPESIndividual::createController(){
    sum_ctrl_freq = 0;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if(ctrlGenome->get_type() == "empty_genome" || drop_learning)
        return;

    const int nn_inputs = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_input();
    const int nn_outputs = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_output();
    const int nb_hidden = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_hidden();
    const int nn_type = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nn_type();

    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

    if(weights.empty() || bias.empty()){
        if(verbose)
            std::cout << "Weights or biases empty! " << weights.empty() << " " << bias.empty() << std::endl;
        return;
    }

    if(nn_type == settings::nnType::FFNN){
        control = std::make_shared<NN2Control<ffnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(nn_inputs,nb_hidden,nn_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::ELMAN){
        control = std::make_shared<NN2Control<elman_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(nn_inputs,nb_hidden,nn_outputs,weights,bias);

    }
    else if(nn_type == settings::nnType::RNN){
        control = std::make_shared<NN2Control<rnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(nn_inputs,nb_hidden,nn_outputs,weights,bias);
    }
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }
}

void M_NIPESIndividual::update(double delta_time){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool use_joint_feedback = settings::getParameter<settings::Boolean>(parameters,"#useJointFeedback").value;
    bool use_wheel_feedback = settings::getParameter<settings::Boolean>(parameters,"#useWheelFeedback").value;
    if(ctrlGenome->get_type() == "empty_genome" || drop_learning || control.get() == nullptr){
        if(verbose)
            std::cout << "drop update : " << ctrlGenome->get_type()
                  << " learning dropped: " << drop_learning
                  << " controller null: " << (control.get() == nullptr) << std::endl;
        return;
    }

    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;
    std::cout << sum_ctrl_freq << " - " << delta_time << std::endl;
    if( fabs(sum_ctrl_freq - ctrl_freq) < 0.0001){
        sum_ctrl_freq = 0;
        //- Retrieve sensors, joints and wheels values
        std::vector<double> inputs = morphology->update();

        if(use_joint_feedback){
            std::vector<double> joints = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_joints_positions();
            for(double &j: joints)
                j = 2.*j/M_PI;
            inputs.insert(inputs.end(),joints.begin(),joints.end());
        }
        if(use_wheel_feedback){
            std::vector<double> wheels = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_wheels_positions();
            inputs.insert(inputs.end(),wheels.begin(),wheels.end());
        }
        std::vector<double> outputs = control->update(inputs);
        morphology->command(outputs);
        energy_cost += std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_energy_cost();
        if(std::isnan(energy_cost))
            energy_cost = 0;
        int morphHandle = std::dynamic_pointer_cast<sim::Morphology>(morphology)->getMainHandle();
        float position[3];
        simGetObjectPosition(morphHandle, -1, position);
        std::cout << "current position: " << position[0] << " "  << position[1] << " " << position[2] << std::endl;
    }

    sum_ctrl_freq += settings::getParameter<settings::Float>(parameters,"#timeStep").value;
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
        if(final_position.empty())
            return Eigen::VectorXd::Zero(3);
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
    oarch.register_type<NN2CPPNGenome>();
    oarch.register_type<EmptyGenome>();
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
    iarch.register_type<NN2CPPNGenome>();
    iarch.register_type<EmptyGenome>();
    iarch.register_type<CMAESLearner>();
    iarch >> *this;

    //set parameters and randNum to the genome as it is not contained in the serialisation
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}

void M_NIPESIndividual::compute_fitness(){
    double fitness = 0;
    for(const auto &r : rewards)
        fitness += r;
    fitness /= static_cast<double>(rewards.size());
    objectives[0] = fitness;
    copy_rewards = rewards;
}

int M_NIPES::novelty_params::k_value = 15;
double M_NIPES::novelty_params::archive_adding_prob = 0.4;
double M_NIPES::novelty_params::novelty_thr = 14641;

M_NIPES::M_NIPES(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) : EA(rn, param){
    settings::defaults::parameters->emplace("#tournamentSize",std::make_shared<const settings::Integer>(4));
    settings::defaults::parameters->emplace("#useControllerArchive",std::make_shared<const settings::Boolean>(true));
    settings::defaults::parameters->emplace("#fitnessType",std::make_shared<const settings::Integer>(0));
    settings::defaults::parameters->emplace("#populationSize",std::make_shared<const settings::Integer>(25));
}

void M_NIPES::init(){
    nn2::rgen_t::gen.seed(randomNum->getSeed());

    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;

    if(!simulator_side || instance_type == settings::INSTANCE_REGULAR){
        int fitness_type = settings::getParameter<settings::Integer>(parameters,"#fitnessType").value;

        if(fitness_type == BEST_FIT)
            fitness_fct = FitnessFunctions::best_fitness;
        else if(fitness_type == AVG_FIT)
            fitness_fct = FitnessFunctions::average_fitness;
        else if(fitness_type == LEARNING_PROG)
            fitness_fct = FitnessFunctions::learning_progress;
        else fitness_fct = FitnessFunctions::best_fitness;

        if(use_ctrl_arch){
            int max_nbr_organs = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;
            controller_archive.init(max_nbr_organs,max_nbr_organs,max_nbr_organs);
        }


        std::string bootstrap_folder = settings::getParameter<settings::String>(parameters,"#bootstrapFolder").value;
        std::string experiment_folder = settings::getParameter<settings::String>(parameters,"#oldExperimentFolder").value;
        std::string seed_file = settings::getParameter<settings::String>(parameters,"#seedMorphGenome").value;

        if(experiment_folder != "None")
            load_experiment(experiment_folder);
        else if(seed_file != "None")
            seed_experiment(seed_file);
        else if(bootstrap_folder != "None")
            bootstrap_evolution(bootstrap_folder);


        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL){
            std::string filename = settings::getParameter<settings::String>(parameters,"#envListFile").value;
            std::string scenes_folder = settings::getParameter<settings::String>(parameters,"#modelsPath").value + "/scenes/";
            sim::GradualEnvironment::load_environments_list(filename,scenes_folder,environments_info);
        }

        bool with_crossover = settings::getParameter<settings::Boolean>(parameters,"#withCrossover").value;
        if(with_crossover) selection_fct = SelectionFunctions::two_best_of_subset;
        else selection_fct = SelectionFunctions::best_of_subset;
        int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        if(gene_pool.size() < pop_size)
            init_morph_pop();
        else reproduction();

    }else if(instance_type == settings::INSTANCE_SERVER && simulator_side){
        EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
        NN2CPPNGenome::Ptr morphgenome = std::make_shared<NN2CPPNGenome>(randomNum,parameters);
        M_NIPESIndividual::Ptr ind = std::make_shared<M_NIPESIndividual>(morphgenome,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void M_NIPES::init_morph_pop(){
    const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    for (size_t i = population.size(); i < population_size - gene_pool.size(); i++){ // Body plans
        EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
        NN2CPPNGenome::Ptr morphgenome = std::make_shared<NN2CPPNGenome>(randomNum,parameters);
        morphgenome->random();
        morphgenome->set_id(highest_morph_id++);

        learner_t new_learner(*morphgenome.get());
        new_learner.ctrl_learner.set_parameters(parameters);
        learning_pool.push_back(new_learner);

        M_NIPESIndividual::Ptr ind = std::make_shared<M_NIPESIndividual>(morphgenome,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::vector<double> init_pos;
        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
            init_pos = environments_info[current_gradual_scene].init_position;
        else
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_init_position(init_pos);
        population.push_back(ind);
    }
}

bool M_NIPES::finish_eval(const Environment::Ptr &env){

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(population[currentIndIndex]->get_ctrl_genome()->get_type() == "empty_genome")
        return true;

    //- Check stopping criteria -

    //Check if the robot is moving
    int handle = std::dynamic_pointer_cast<CPPNMorph>(population[currentIndIndex]->get_morphology())->getMainHandle();
    float pos[3];
    simGetObjectPosition(handle,-1,pos);
    float sim_time = simGetSimulationTime();
    if(sim_time < 0.1){
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
    if(drop_eval)
        std::dynamic_pointer_cast<M_NIPESIndividual>(population[currentIndIndex])->incr_nbr_dropped_eval();


    //Check if the fitness target has been reached
    std::vector<double> fitness;
    if(env->get_name() == "mazeEnv")
        fitness = std::dynamic_pointer_cast<sim::MazeEnv>(env)->fitnessFunction(population[currentIndIndex]);
    else if(env->get_name() == "obstacle_avoidance")
        fitness = std::dynamic_pointer_cast<sim::ObstacleAvoidance>(env)->fitnessFunction(population[currentIndIndex]);
    else if(env->get_name() == "exploration")
        fitness = std::dynamic_pointer_cast<sim::Exploration>(env)->fitnessFunction(population[currentIndIndex]);
    else if(env->get_name() == "gradual_tasks")
        fitness = std::dynamic_pointer_cast<sim::GradualEnvironment>(env)->fitnessFunction(population[currentIndIndex]);
    else if(env->get_name() == "multi_target_maze")
        fitness = std::dynamic_pointer_cast<sim::MultiTargetMaze>(env)->fitnessFunction(population[currentIndIndex]);
    else if(env->get_name() == "barrel_task")
        fitness = std::dynamic_pointer_cast<sim::BarrelTask>(env)->fitnessFunction(population[currentIndIndex]);

    double fitness_target = 1 - settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    if(env->get_name() == "gradual_tasks"){
        std::vector<sim::GradualEnvironment::env_t> environments_info = std::dynamic_pointer_cast<sim::GradualEnvironment>(env)->get_environments_info();
        fitness_target = environments_info[current_gradual_scene].fitness_target;
    }

    bool target_reached = fitness[0] > fitness_target;

    bool stop = drop_eval || target_reached;
    if(env->get_name() == "gradual_tasks"){
        std::vector<sim::GradualEnvironment::env_t> environments_info = std::dynamic_pointer_cast<sim::GradualEnvironment>(env)->get_environments_info();
        stop = stop || sim_time >= environments_info[current_gradual_scene].max_eval_time;
    }

    if(stop && verbose){
        if(env->get_name() == "gradual_tasks")
            std::cout << "stop eval: " << "fitness target reached: " << target_reached
                  << " eval dropped " << drop_eval
                  << " sim_time " << (sim_time >= environments_info[current_gradual_scene].max_eval_time) << std::endl;
        else
            std::cout << "stop eval: " << "fitness target reached: " << target_reached
                  << " eval dropped " << drop_eval << std::endl;
    }

    return stop;
}

bool M_NIPES::is_finish(){
    int max_nbr_eval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    bool fullfil_all_tasks = false;
    if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
        fullfil_all_tasks = current_gradual_scene > environments_info.size();
    if(numberEvaluation >= max_nbr_eval  || fullfil_all_tasks){
        if(settings::getParameter<settings::Boolean>(parameters,"#computeEvolvability").value){
            int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
            std::cout << "evolvability: " << evolvability_score/static_cast<double>(pop_size) << std::endl;
        }
        return true;
    }
    return false;
}

void M_NIPES::init_next_pop(){
    clean_learning_pool();

    if(learning_pool.empty())
        reproduction();
    else{
        for(auto& learner: learning_pool)
            if(!learner.ctrl_learner.is_learning_finish() && learner.ctrl_learner.is_initialized())
                init_new_ctrl_pop(learner);
        if(population.empty())
            for(auto& learner: learning_pool)
                if(!learner.ctrl_learner.is_learning_finish() && learner.ctrl_learner.is_initialized())
                    push_back_remaining_ctrl(learner);
    }
    assert(population.check_doublons() == false);

}

bool M_NIPES::update(const Environment::Ptr &env){
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    std::cout << "Population : " << population.size() << " - learning pool : " << learning_pool.size() << " - gene pool : " << gene_pool.size() << std::endl;
    std::cout << "newly evaluated : " << newly_evaluated.size() << std::endl;

    clean_learning_pool();

    for(int index : newly_evaluated){
        try{
            Individual::Ptr ind = population[index];
            if((instance_type == settings::INSTANCE_SERVER && simulator_side) || instance_type == settings::INSTANCE_REGULAR){
                std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_final_position(env->get_final_position());
                if(env->get_name() == "obstacle_avoidance"){
                    std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_visited_zones(std::dynamic_pointer_cast<sim::ObstacleAvoidance>(env)->get_visited_zone_matrix());
                    std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_trajectories({env->get_trajectory()});
                    std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_descriptor_type(VISITED_ZONES);
                }
                else if(env->get_name() == "mazeEnv"){
                    std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_trajectories({env->get_trajectory()});
                }else{
                    std::cerr << "task unknown" << std::endl;
                    exit(1);
                }
            }

            //If on the client or just sequential mode
            if((instance_type == settings::INSTANCE_SERVER && !simulator_side) || instance_type == settings::INSTANCE_REGULAR){
                int morph_id = std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->id();
                try{
                    learner_t &learner = find_learner(morph_id);

                    learner.ctrl_learner.set_nbr_dropped_eval(std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->get_nbr_dropped_eval());
                    if(ind->get_ctrl_genome()->get_type() == "empty_genome"){//if ctrl genome is empty
                        learner.morph_genome.set_cart_desc(std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->get_cart_desc());
                        learner.morph_genome.set_organ_position_desc(std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->get_organ_position_desc());
                        int wheel_nbr = learner.morph_genome.get_cart_desc().wheelNumber;
                        int joint_nbr = learner.morph_genome.get_cart_desc().jointNumber;
                        int sensor_nbr = learner.morph_genome.get_cart_desc().sensorNumber;
                        if(wheel_nbr > 0 || joint_nbr > 0){
                            init_new_learner(learner.ctrl_learner,wheel_nbr,joint_nbr,sensor_nbr);
                            init_new_ctrl_pop(learner);
                        }else{ //if(std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->get_parents_ids()[0] == -1 &&
                            //   std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->get_parents_ids()[1] == -1){
                            //if this robot has no actuators and  has no parents (from first generation), it is not included in the genomes pool and it is replaced by a new random one
                            learner.ctrl_learner.to_be_erased();
                            EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
                            NN2CPPNGenome::Ptr morphgenome = std::make_shared<NN2CPPNGenome>(randomNum,parameters);
                            morphgenome->random();
                            morphgenome->set_id(highest_morph_id++);

                            learner_t new_learner(*(morphgenome.get()));
                            new_learner.ctrl_learner.set_parameters(parameters);
                            learning_pool.push_back(new_learner);

                            M_NIPESIndividual::Ptr ind = std::make_shared<M_NIPESIndividual>(morphgenome,ctrl_gen);
                            ind->set_parameters(parameters);
                            ind->set_randNum(randomNum);
                            population.push_back(ind);
                        }

                    }else if(std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->is_learning_dropped()){
                        if(verbose)
                            std::cout << "learner " << morph_id << "learning dropped" << std::endl;
                        learner.ctrl_learner.to_be_erased();
                    }else{
                        numberEvaluation++;
                        nbr_eval_current_task++;
                        //update learner
                        auto trajs = std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->get_trajectories();
                        int env_type = are::settings::getParameter<are::settings::Integer>(parameters,"#envType").value;
                        if(env_type == are::OBSTACLES && ind->descriptor().rows() != 64){
                            std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_descriptor_type(VISITED_ZONES);
                            std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_visited_zones(Eigen::MatrixXi::Zero(8,8));
                        }
                        learner.ctrl_learner.update_pop_info(ind->getObjectives(),ind->descriptor(),trajs);
                        bool is_ctrl_next_gen = learner.ctrl_learner.step();
                        //-

                        if(verbose && is_ctrl_next_gen){
                            std::cout << "Morphology " << learner.morph_genome.id() << " in learning "
                                      << learner.ctrl_learner.nbr_eval() << "/" << settings::getParameter<settings::Integer>(parameters,"#cmaesLargeNbrEval").value;
                            if(!learner.ctrl_learner.get_best_solution().objectives.empty())
                                std::cout << " - Best fitness so far : " << learner.ctrl_learner.get_best_solution().objectives[0] << std::endl;
                            else std::cout << std::endl;
                        }



                        if(learner.ctrl_learner.is_learning_finish()){//learning is finished for this body plan
                            //Update Controller Archive
                            std::vector<double> weights;
                            std::vector<double> biases;
                            NNParamGenome best_ctrl_gen;
                            auto &best_controller = learner.ctrl_learner.get_best_solution();

                            int nbr_weights = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_weights().size();
                            int nbr_biases = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_biases().size();
                            int nbr_inputs = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_nbr_input();
                            int nbr_outputs = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_nbr_output();
                            int nbr_hidden = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_nbr_hidden();
                            int nn_type = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_nn_type();

                            if(!best_controller.genome.empty() && best_controller.genome.size() == nbr_weights + nbr_biases){
                                weights.resize(nbr_weights);
                                for(size_t i = 0; i < nbr_weights; i++)
                                    weights[i] = best_controller.genome[i];
                                biases.resize(nbr_biases);
                                for(size_t i = nbr_weights; i < best_controller.genome.size(); i++)
                                    biases[i-nbr_weights] = best_controller.genome[i];
                                best_ctrl_gen.set_weights(weights);
                                best_ctrl_gen.set_biases(biases);
                                best_ctrl_gen.set_nbr_input(nbr_inputs);
                                best_ctrl_gen.set_nbr_output(nbr_outputs);
                                best_ctrl_gen.set_nbr_hidden(nbr_hidden);
                                best_ctrl_gen.set_nn_type(nn_type);
                                //update the archive
                                if(use_ctrl_arch){
                                    CartDesc morph_desc = learner.morph_genome.get_cart_desc();
                                    controller_archive.update(std::make_shared<NNParamGenome>(best_ctrl_gen),best_controller.objectives[0],morph_desc.wheelNumber,morph_desc.jointNumber,morph_desc.sensorNumber);
                                }
                                //-
                            }


                            //add new gene in gene_pool
                            genome_t new_gene(learner.morph_genome,best_ctrl_gen,{fitness_fct(learner.ctrl_learner)},learner.ctrl_learner.learning_progress());
                            new_gene.trajectories = best_controller.trajectories;
                            misc::stdvect_to_eigenvect(best_controller.descriptor,new_gene.behavioral_descriptor);
                            new_gene.nbr_eval =  learner.ctrl_learner.get_nbr_eval();
                            gene_pool.push_back(new_gene);
                            //-

                            if(verbose){
                                int max_nbr_eval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
                                if(warming_up)
                                    std::cout << "warming up phase : " << gene_pool.size() << " out of " << pop_size << " morphologies." << std::endl;
                                else
                                    std::cout << "main phase : " << numberEvaluation << " out of " << max_nbr_eval << " evaluations." << std::endl;
                                std::cout << "Morphology " << learner.morph_genome.id() << " finish learning - best fitness : "
                                          << learner.ctrl_learner.get_best_solution().objectives[0]
                                          << " - learning progress : " << learner.ctrl_learner.learning_progress() <<  std::endl;
                            }


                            //level of synchronicity. 1.0 fully synchrone, 0.0 fully asynchrone. Result to the number of offsprings to be evaluated before generating new offsprings
                            int nbr_offsprings = static_cast<int>(pop_size*settings::getParameter<settings::Float>(parameters,"#synchronicity").value);
                            if(nbr_offsprings == 0) nbr_offsprings = 1; //Fully asynchronous
                            if(warming_up && gene_pool.size() == pop_size){// Warming up phase finished.
                                warming_up = false;
                                increment_age();
                                reproduction();
                                assert(learning_pool.size() == pop_size);
                            }
                            //Perform survival and selection and generate a new morph gene.
                            else if(gene_pool.size() == pop_size+nbr_offsprings){
                                //remove oldest gene and increase age
                                std::string survival = settings::getParameter<settings::String>(parameters,"#survivalMethod").value;
                                while(gene_pool.size() > pop_size){
                                    if(survival == "oldest")
                                        remove_oldest_gene();
                                    else if(survival == "worst")
                                        remove_worst_gene();
                                    else std::cerr << "WARNING: Unknown survival method, so no survival applied" << std::endl;
                                }
                                increment_age();
                                //-
                               // assert(gene_pool.size() == pop_size);
                                reproduction();
                               // assert(learning_pool.size() == pop_size);
                            }
                            //-
                        }else if(is_ctrl_next_gen){ //if NIPES goes for a next gen
                            init_new_ctrl_pop(learner);
                        }
                    }
                }catch(std::invalid_argument &e){
                    std::cerr << e.what() << " id " << morph_id << std::endl;
                }

                population.erase(index);
                //population.shrink_to_fit();
            }
            ind.reset();
        }catch(std::invalid_argument &e){
            std::cerr << e.what() << " id " << index << std::endl;
            continue;
        }
    }
    newly_evaluated.clear();
    return true;
}

//void M_NIPES::update_pools(){
//    //add new gene in gene_pool
//    genome_t new_gene(learner.morph_genome,best_ctrl_gen,{fitness_fct(learner.ctrl_learner)});
//    new_gene.trajectories = best_controller.trajectories;
//    misc::stdvect_to_eigenvect(best_controller.descriptor,new_gene.behavioral_descriptor);
//    new_gene.nbr_eval =  learner.ctrl_learner.get_nbr_eval();
//    gene_pool.push_back(new_gene);
//    //-

//    if(verbose){
//        if(warming_up)
//            std::cout << "warming up phase : " << gene_pool.size() << " out of " << pop_size << std::endl;
//        else
//            std::cout << "main phase : " << std::endl;
//        std::cout << "Morphology " << learner.morph_genome.id() << " finish learning" << std::endl;
//    }


//    //level of synchronicity. 1.0 fully synchrone, 0.0 fully asynchrone. Result to the number of offsprings to be evaluated before generating new offsprings
//    int nbr_offsprings = static_cast<int>(pop_size*settings::getParameter<settings::Float>(parameters,"#synchronicity").value);
//    if(nbr_offsprings == 0) nbr_offsprings = 1; //Fully asynchronous
//    if(warming_up && gene_pool.size() == pop_size){// Warming up phase finished.
//        warming_up = false;
//        increment_age();
//        reproduction();
//        assert(learning_pool.size() == pop_size);
//    }
//    //Perform survival and selection and generate a new morph gene.
//    else if(gene_pool.size() == pop_size+nbr_offsprings){
//        //remove oldest gene and increase age
//        std::string survival = settings::getParameter<settings::String>(parameters,"#survivalMethod").value;
//        while(gene_pool.size() > pop_size){
//            if(survival == "oldest")
//                remove_oldest_gene();
//            else if(survival == "worst")
//                remove_worst_gene();
//            else std::cerr << "WARNING: Unknown survival method, so no survival applied" << std::endl;
//        }
//        increment_age();
//        //-
//        assert(gene_pool.size() == pop_size);
//        reproduction();
//        assert(learning_pool.size() == pop_size);
//    }
//    //-
//}

void M_NIPES::reproduction(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    int tournament_size = settings::getParameter<settings::Integer>(parameters,"#tournamentSize").value;
    int obj_idx = settings::getParameter<settings::Integer>(parameters,"#morphSelectionObjective").value;

    if(obj_idx == 1)
        compute_novelty_scores();

    while(learning_pool.size() < pop_size){ //create offspring until refilling entirely the learning pool
        //Random selection of indexes without duplicate
        std::vector<int> random_indexes;
        random_indexes.push_back(randomNum->randInt(0,gene_pool.size()-1));
        do{
            int rand_idx = randomNum->randInt(0,gene_pool.size()-1);
            bool already_drawn = false;
            for(const int& idx: random_indexes)
                if(idx == rand_idx){
                    already_drawn = true;
                    break;
                }
            if(!already_drawn)
                random_indexes.push_back(rand_idx);
        }while(random_indexes.size() < tournament_size);
        //-

        //Add these new gene to learning pool
        std::vector<genome_t> gene_subset;
        for(const int &idx: random_indexes)
            gene_subset.push_back(gene_pool[idx]);
        NN2CPPNGenome new_morph_gene = selection_fct(gene_subset);
        new_morph_gene.set_id(highest_morph_id++);
        new_morph_gene.set_parameters(parameters);
        new_morph_gene.set_randNum(randomNum);
        learner_t new_learner(new_morph_gene);
        new_learner.ctrl_learner.set_parameters(parameters);
        learning_pool.push_back(new_learner);
        //-

        //Add it to the population with an empty ctrl genome to be build and determine its type.
        NN2CPPNGenome::Ptr morph_genome = std::make_shared<NN2CPPNGenome>(new_morph_gene);
        EmptyGenome::Ptr ctrl_genome = std::make_shared<EmptyGenome>();
        M_NIPESIndividual::Ptr ind = std::make_shared<M_NIPESIndividual>(morph_genome,ctrl_genome);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::vector<double> init_pos;
        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
            init_pos = environments_info[current_gradual_scene].init_position;
        else
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_init_position(init_pos);
        population.push_back(ind);
        //-
    }
}

void M_NIPES::compute_novelty_scores(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    if(gene_pool.size() < pop_size)
        return;

    std::vector<Eigen::VectorXd> genes_desc;
    for(auto& gene :gene_pool)
        genes_desc.push_back(gene.morph_genome.get_organ_position_desc().getCartDesc());
    for(auto& gene :gene_pool){
        Eigen::VectorXd desc = gene.morph_genome.get_organ_position_desc().getCartDesc();
        std::vector<double> dists = Novelty::distances(desc,novelty_archive,genes_desc,Novelty::distance_fcts::positional);
        gene.objectives.resize(2);
        gene.objectives[1] = novelty::sparseness<novelty_params>(dists);
    }
    bool with_archive = settings::getParameter<settings::Boolean>(parameters,"#morphNoveltyWithArchive").value;
    if(with_archive){
        for(auto& gene :gene_pool){
            Eigen::VectorXd desc = gene.morph_genome.get_organ_position_desc().getCartDesc();
            novelty::update_archive<novelty_params>(desc,gene.objectives[1],novelty_archive,randomNum);
        }
    }


}



genome_t& M_NIPES::find_gene(int id){
    for(auto& gene: gene_pool)
        if(gene.morph_genome.id() == id)
            return gene;
    throw std::invalid_argument("M_NIPES::find_gene: not found");

    //std::cerr << "Unable to find genome with id: " << id << std::endl;
    //return boost::optional<genome_t&>();
}


learner_t& M_NIPES::find_learner(int id){
    for(auto& learner: learning_pool)
        if(learner.morph_genome.id() == id)
            return learner;
    throw std::invalid_argument("M_NIPES::find_learner: not found");
    //std::cerr << "Unable to find learner with id: " << id << std::endl;
    //return boost::optional<learner_t&>();
}

void M_NIPES::remove_learner(int id){
    int i = 0;
    for(; i < learning_pool.size(); i++)
        if(id == learning_pool[i].morph_genome.id())
            break;
    learning_pool.erase(learning_pool.begin() + i);
}

void M_NIPES::remove_oldest_gene(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    if(gene_pool.size() <= pop_size)
        return;
    //first gather all the oldest genes
    std::vector<int> oldest_gene_idx;
    
    for(int i = 0; i < gene_pool.size(); i++)
        if(gene_pool[i].age == highest_age)
            oldest_gene_idx.push_back(i);
    while(oldest_gene_idx.empty()){//if no more genomes of the highest age remain decrement it and search again.
	highest_age--;
        for(int i = 0; i < gene_pool.size(); i++)
            if(gene_pool[i].age == highest_age)
               oldest_gene_idx.push_back(i);
    }
    //-

    //then erase one selected randomly among the oldest.
    gene_pool.erase(gene_pool.begin() + oldest_gene_idx[randomNum->randInt(0,oldest_gene_idx.size()-1)]);
}

void M_NIPES::remove_worst_gene(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    if(gene_pool.size() <= pop_size)
        return;

    //find lowest fitness value in the genome pool
    double lowest_obj = gene_pool[0].objectives[0];
    size_t worst_gene_idx = 0;
    for(size_t i = 1; i < gene_pool.size(); i++){
        if(lowest_obj > gene_pool[i].objectives[0]){
            lowest_obj = gene_pool[i].objectives[0];
            worst_gene_idx = i;
        }
    }
    //-

    //then erase the worst one.
    gene_pool.erase(gene_pool.begin() + worst_gene_idx);
}

void M_NIPES::increment_age(){
    highest_age++;
    for(auto& gene: gene_pool)
        gene.age++;
}

void M_NIPES::clean_learning_pool(){
    std::vector<int> elt_to_remove;
    for(const learner_t& l : learning_pool){
        if(l.ctrl_learner.is_initialized() && l.ctrl_learner.is_learning_finish()){
            std::cout << "removing learner id: " << l.morph_genome.id() << std::endl;
            elt_to_remove.push_back(l.morph_genome.id());
        }
    }
    for(const int& id : elt_to_remove){
        remove_learner(id);
    }
}



void M_NIPES::init_new_learner(CMAESLearner &learner, const int wheel_nbr, int joint_nbr, int sensor_nbr){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    bool use_camera = settings::getParameter<settings::Boolean>(parameters,"#useCamera").value;
    bool use_ir = settings::getParameter<settings::Boolean>(parameters,"#useIR").value;
    bool use_joint_feedback = settings::getParameter<settings::Boolean>(parameters,"#useJointFeedback").value;
    bool use_wheel_feedback = settings::getParameter<settings::Boolean>(parameters,"#useWheelFeedback").value;
    int nn_inputs = sensor_nbr + //proximity sensors
            (use_ir ? sensor_nbr : 0) + // IR sensors
            (use_joint_feedback ? joint_nbr : 0) + // joint positions
            (use_wheel_feedback ? wheel_nbr : 0) + // wheel positions
            (use_camera ? 1 : 0); // camera

    int nn_outputs = wheel_nbr + joint_nbr;

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

    learner = CMAESLearner(nbr_weights, nbr_bias,nn_inputs,nn_outputs);
    learner.set_parameters(parameters);
    learner.set_randNum(randomNum);

    double ftarget;
    if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
        ftarget = 1 - environments_info[current_gradual_scene].fitness_target;
    else ftarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;


    if(use_ctrl_arch){
        auto& starting_gen = controller_archive.archive[wheel_nbr][joint_nbr][sensor_nbr].first;


        if(starting_gen->get_weights().empty() && starting_gen->get_biases().empty())
            learner.init(ftarget);
        else{
            std::vector<double> init_pt = std::dynamic_pointer_cast<NNParamGenome>(starting_gen)->get_full_genome();
            double starting_fit = controller_archive.archive[wheel_nbr][joint_nbr][sensor_nbr].second;
            learner.init(ftarget,init_pt,starting_fit);
        }
    }else learner.init(ftarget);
}

void M_NIPES::init_new_ctrl_pop(learner_t &learner){
    auto new_ctrl_pop = learner.ctrl_learner.get_new_population();
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;

    for(const auto &wb : new_ctrl_pop){
        NN2CPPNGenome::Ptr morph_gen = std::make_shared<NN2CPPNGenome>(learner.morph_genome);
        NNParamGenome::Ptr ctrl_gen = std::make_shared<NNParamGenome>(randomNum,parameters,morph_gen->id());
        ctrl_gen->set_weights(wb.first);
        ctrl_gen->set_biases(wb.second);
        ctrl_gen->set_nbr_hidden(nb_hidden);
        ctrl_gen->set_nbr_output(learner.ctrl_learner.get_nbr_outputs());
        ctrl_gen->set_nbr_input(learner.ctrl_learner.get_nbr_inputs()); // Two per multi-sensor + 1 camera
        ctrl_gen->set_nn_type(nn_type);
        Individual::Ptr ind = std::make_shared<M_NIPESIndividual>(morph_gen,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_ctrl_archive(controller_archive);
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_current_gradual_scene(current_gradual_scene);
        std::vector<double> init_pos;
        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
            init_pos = environments_info[current_gradual_scene].init_position;
        else
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_init_position(init_pos);
        population.push_back(ind);
    }
}

void M_NIPES::push_back_remaining_ctrl(learner_t &learner){
    auto new_ctrl_pop = learner.ctrl_learner.get_remaining_population();
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;

    for(const auto &wb : new_ctrl_pop){
        NN2CPPNGenome::Ptr morph_gen = std::make_shared<NN2CPPNGenome>(learner.morph_genome);
        NNParamGenome::Ptr ctrl_gen = std::make_shared<NNParamGenome>(randomNum,parameters,morph_gen->id());
        ctrl_gen->set_weights(wb.first);
        ctrl_gen->set_biases(wb.second);
        ctrl_gen->set_nbr_hidden(nb_hidden);
        ctrl_gen->set_nbr_output(learner.morph_genome.get_cart_desc().wheelNumber + learner.morph_genome.get_cart_desc().jointNumber);
        ctrl_gen->set_nbr_input(learner.morph_genome.get_cart_desc().sensorNumber*2+1); // Two per multi-sensor + 1 camera
        ctrl_gen->set_nn_type(nn_type);
        Individual::Ptr ind = std::make_shared<M_NIPESIndividual>(morph_gen,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_current_gradual_scene(current_gradual_scene);
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_ctrl_archive(controller_archive);
        std::vector<double> init_pos;
        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
            init_pos = environments_info[current_gradual_scene].init_position;
        else
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_init_position(init_pos);

        population.push_back(ind);
    }
}

void M_NIPES::incr_gradual_scene(){
    current_gradual_scene++;
    for(auto& ind : population)
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->incr_gradual_scene();
}


void M_NIPES::bootstrap_evolution(const std::string &folder){
    //Step One: fill the population with the morph_genome from the folder
    std::string filepath, filename;
    std::vector<std::string> split_str;
    int i = 0;
    for(const auto &dirit : fs::directory_iterator(fs::path(folder))){
        filepath = dirit.path().string();
        misc::split_line(filepath,"/",split_str);
        filename = split_str.back();
        misc::split_line(filename,"_",split_str);
        if(split_str.front() != "morph" || split_str[1] != "genome" )
            continue;
        nn2_cppn_t cppn;
        std::ifstream ifs(filepath);
        boost::archive::text_iarchive iarch(ifs);
        iarch >> cppn;
        NN2CPPNGenome::Ptr morphgenome = std::make_shared<NN2CPPNGenome>(cppn);
        morphgenome->set_id(highest_morph_id++);
        morphgenome->set_randNum(randomNum);
        morphgenome->set_parameters(parameters);
        learner_t new_learner(*morphgenome.get());
        new_learner.ctrl_learner.set_parameters(parameters);
        learning_pool.push_back(new_learner);

        EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
        M_NIPESIndividual::Ptr ind = std::make_shared<M_NIPESIndividual>(morphgenome,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::vector<double> init_pos;
        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
            init_pos = environments_info[current_gradual_scene].init_position;
        else
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_init_position(init_pos);
        population.push_back(ind);
        i++;
    }

    //Step Two: load controller archive
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    if(use_ctrl_arch){
        filename = folder + std::string("/controller_archive");
        controller_archive.from_file(filename);
    }
}

void M_NIPES::load_experiment(const std::string &folder){
    std::ifstream ifs(folder + "/genomes_pool.csv");
    if(!ifs){
        std::cerr << "unable to open file " << folder << "/genomes_pool.csv" << std::endl;
        exit(1);
    }
    std::string line;
    std::vector<std::string> ids;
    while(std::getline(ifs,line))
        misc::split_line(line,",",ids);
    ifs.close();

    //load fitness file
    std::ifstream fit_ifs(folder + "/fitnesses.csv");
    if(!fit_ifs){
        std::cerr << "unable to open file " << folder << "/fitnesses.csv" << std::endl;
        exit(1);
    }
    std::vector<std::string> split_line;
    std::map<int,float> fitnesses;
    highest_morph_id = 0;
    while(std::getline(fit_ifs,line)){
        misc::split_line(line,",",split_line);
        fitnesses.emplace(std::stoi(split_line[0]),std::stof(split_line[3]));
        if(std::stoi(split_line[0]) > highest_morph_id)
            highest_morph_id = std::stoi(split_line[0]);
    }
    fit_ifs.close();

    for(const std::string &id : ids){
        //Load morph genome
        nn2_cppn_t cppn;
        std::string morph_file = folder + "/morph_genome_" + id;
        std::ifstream mifs(morph_file);
        if(!mifs){
            std::cerr << "unable to open file " << morph_file << std::endl;
            exit(1);
        }
        boost::archive::text_iarchive miarch(mifs);
        miarch >> cppn;
        mifs.close();
        NN2CPPNGenome morph_genome(cppn);
        morph_genome.set_id(std::stoi(id));
        morph_genome.set_randNum(randomNum);
        morph_genome.set_parameters(parameters);

        //Load ctrl genome
        std::string ctrl_file = folder + "/ctrl_genome_" + id;
        NNParamGenome ctrl_genome;
        ctrl_genome.from_file(ctrl_file);
        ctrl_genome.set_randNum(randomNum);
        ctrl_genome.set_parameters(parameters);


        //load the trajectory.
        std::string traj_file = folder + "/traj_" + id + "_0";
        std::ifstream tifs(traj_file);
        if(!tifs){
            std::cerr << "unable to open file " << traj_file << std::endl;
            exit(1);
        }
        std::vector<waypoint> trajectory;
        while(std::getline(tifs,line)){
            waypoint wp;
            wp.from_string(line);
            trajectory.push_back(wp);
        }
        tifs.close();

        //add new gene in gene_pool
        genome_t new_gene(morph_genome,ctrl_genome,{fitnesses[std::stoi(id)]});
        new_gene.trajectories = {trajectory};
        gene_pool.push_back(new_gene);
        //-
    }

    //load controller archive
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    if(use_ctrl_arch)
        controller_archive.from_file(folder + "/controller_archive");

}

void M_NIPES::seed_experiment(const std::string &morph_file){

    //Load morph genome
    std::ifstream mifs(morph_file);
    if(!mifs){
        std::cerr << "unable to open file " << morph_file << std::endl;
        exit(1);
    }
    boost::archive::text_iarchive miarch(mifs);
    miarch >> seed_cppn;
    mifs.close();

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    if(settings::getParameter<settings::Boolean>(parameters,"#computeEvolvability").value){
        NN2CPPNGenome::Ptr morphgenome = std::make_shared<NN2CPPNGenome>(seed_cppn);
        morphgenome->set_id(highest_morph_id++);
        morphgenome->set_randNum(randomNum);
        morphgenome->set_parameters(parameters);
        learner_t new_learner(*morphgenome.get());
        new_learner.ctrl_learner.set_parameters(parameters);
        learning_pool.push_back(new_learner);

        EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
        M_NIPESIndividual::Ptr ind = std::make_shared<M_NIPESIndividual>(morphgenome,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::vector<double> init_pos;
        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
            init_pos = environments_info[current_gradual_scene].init_position;
        else
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_init_position(init_pos);
        population.push_back(ind);
    }
    for(int i = 0; i < pop_size; i++){
        nn2_cppn_t new_cppn = seed_cppn;
        new_cppn.mutate();
        NN2CPPNGenome::Ptr morphgenome = std::make_shared<NN2CPPNGenome>(new_cppn);
        morphgenome->set_id(highest_morph_id++);
        morphgenome->set_randNum(randomNum);
        morphgenome->set_parameters(parameters);
        learner_t new_learner(*morphgenome.get());
        new_learner.ctrl_learner.set_parameters(parameters);
        learning_pool.push_back(new_learner);

        EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
        M_NIPESIndividual::Ptr ind = std::make_shared<M_NIPESIndividual>(morphgenome,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::vector<double> init_pos;
        if(settings::getParameter<settings::Integer>(parameters,"#envType").value == GRADUAL)
            init_pos = environments_info[current_gradual_scene].init_position;
        else
            init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_init_position(init_pos);
        population.push_back(ind);
    }


}

void M_NIPES::fill_ind_to_eval(std::vector<int> &ind_to_eval){
    for(int i = 0; i < population.size(); i++)
        ind_to_eval.push_back(population.get_index(i));
}

std::string M_NIPES::_task_name(are::task_t task){
    if(task == MAZE)
        return "maze";
    else if(task == OBSTACLES)
        return "obstacles";
    else if(task == MULTI_TARGETS)
        return "multi targets";
    else if(task == EXPLORATION)
        return "exploration";
    else if(task == BARREL)
        return "barrel";
    else if(task == GRADUAL)
        return "gradual";
    else{
        std::cerr << "Error: task unknow" << std::endl;
        return "None";
    }
}
