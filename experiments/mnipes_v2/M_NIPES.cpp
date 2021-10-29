#include "M_NIPES.hpp"

using namespace are;

fitness_fct_t FitnessFunctions::best_fitness = [](const CMAESLearner& learner) -> double
{
    return 1 - learner.get_best_solution().first;
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
    double best_fitness = gene_pool[0].objectives[0];
    int best_idx = 0;
    for(int i = 1; i < gene_pool.size(); i++){
        if(best_fitness < gene_pool[i].objectives[0]){
            best_fitness = gene_pool[i].objectives[0];
            best_idx = i;
        }
    }
    nn2_cppn_t cppn = gene_pool[best_idx].morph_genome.get_cppn();
    cppn.mutate();
    NN2CPPNGenome new_gene(cppn);
    new_gene.set_parents_ids({gene_pool[best_idx].morph_genome.id()});
    return new_gene;
};

void M_NIPESIndividual::createMorphology(){
    individual_id = morphGenome->id();
    morphology.reset(new sim::Morphology_CPPNMatrix(parameters));
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,init_z);
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_morph_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());
    setMorphDesc();
    setManRes();
}

void M_NIPESIndividual::createController(){

    if(ctrlGenome->get_type() == "empty_genome")
        return;

    int nn_inputs = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_input();
    int nn_outputs = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_output();
    int nb_hidden = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nbr_hidden();
    int nn_type = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_nn_type();

    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

    if(nn_type == settings::nnType::FFNN){
        control.reset(new NN2Control<ffnn_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(nn_inputs,nb_hidden,nn_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::ELMAN){
        control.reset(new NN2Control<elman_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(nn_inputs,nb_hidden,nn_outputs,weights,bias);

    }
    else if(nn_type == settings::nnType::RNN){
        control.reset(new NN2Control<rnn_t>());
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

    if(ctrlGenome->get_type() == "empty_genome")
        return;

    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);

    morphology->command(outputs);
    energy_cost += std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_energy_cost();
    if(std::isnan(energy_cost))
        energy_cost = 0;
    sim_time = delta_time;
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
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    Eigen::VectorXd desc(3);
    desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
    return desc;
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


void M_NIPES::init(){

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

        selection_fct = SelectionFunctions::best_of_subset;
        const int population_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        for (size_t i = 0; i < population_size; i++){ // Body plans
            EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
            NN2CPPNGenome::Ptr morphgenome(new NN2CPPNGenome(randomNum,parameters));
            morphgenome->random();
            learner_t new_learner(*morphgenome.get());
            new_learner.ctrl_learner.set_parameters(parameters);
            learning_pool.push_back(new_learner);

            M_NIPESIndividual::Ptr ind(new M_NIPESIndividual(morphgenome,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
            corr_indexes.push_back(i);
        }
    }else if(instance_type == settings::INSTANCE_SERVER && simulator_side){
        EmptyGenome::Ptr ctrl_gen(new EmptyGenome);
        NN2CPPNGenome::Ptr morphgenome(new NN2CPPNGenome(randomNum,parameters));
        M_NIPESIndividual::Ptr ind(new M_NIPESIndividual(morphgenome,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

bool M_NIPES::finish_eval(const Environment::Ptr &env){
    if(population[currentIndIndex]->get_ctrl_genome()->get_type() == "empty_genome")
        return true;

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

    std::vector<double> target = {settings::getParameter<settings::Double>(parameters,"#target_x").value,
                                  settings::getParameter<settings::Double>(parameters,"#target_y").value,
                                  settings::getParameter<settings::Double>(parameters,"#target_z").value};

    float tPos[3];
    tPos[0] = static_cast<float>(target[0]);
    tPos[1] = static_cast<float>(target[1]);
    tPos[2] = static_cast<float>(target[2]);
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

    return stop;
}

bool M_NIPES::is_finish(){
    int max_nbr_eval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    if(numberEvaluation >= max_nbr_eval)
        return true;
    return false;
}

bool M_NIPES::update(const Environment::Ptr &env){
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    clean_learning_pool();

    Individual::Ptr ind;
    if((instance_type == settings::INSTANCE_SERVER && simulator_side) || instance_type == settings::INSTANCE_REGULAR){
        ind = population[currentIndIndex];
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_final_position(env->get_final_position());
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_trajectory(env->get_trajectory());;
    }
    //If one the client or just sequential mode
    if((instance_type == settings::INSTANCE_SERVER && !simulator_side) || instance_type == settings::INSTANCE_REGULAR){
        ind = population[corr_indexes[currentIndIndex]];
        int morph_id = std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->id();
        learner_t &learner = find_learner(morph_id);
        if(ind->get_ctrl_genome()->get_type() == "empty_genome"){//if ctrl genome is empty
            learner.morph_genome.set_morph_desc(std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->get_morph_desc());
            int wheel_nbr = learner.morph_genome.get_morph_desc().wheelNumber;
            int joint_nbr = learner.morph_genome.get_morph_desc().jointNumber;
            int sensor_nbr = learner.morph_genome.get_morph_desc().sensorNumber;
            if(wheel_nbr > 0 || joint_nbr > 0){
                init_new_learner(learner.ctrl_learner,wheel_nbr,joint_nbr,sensor_nbr);
                init_new_ctrl_pop(learner);
            }else{
                genome_t new_gene(learner.morph_genome,NNParamGenome(),{0});
                gene_pool.push_back(new_gene);
                learner.ctrl_learner.to_be_erased();
            }
        }else{
            numberEvaluation++;
            //update learner
            learner.ctrl_learner.update_pop_info(ind->getObjectives(),ind->descriptor());
            bool is_ctrl_next_gen = learner.ctrl_learner.step();
            //-

            if(learner.ctrl_learner.is_learning_finish()){//learning is finished for this body plan

                //Update Controller Archive
                std::vector<double> weights;
                std::vector<double> biases;
                NNParamGenome best_ctrl_gen;
                auto &best_controller = learner.ctrl_learner.get_best_solution();
                int nbr_weights = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_weights().size();
                weights.insert(weights.end(),best_controller.second.begin(),best_controller.second.begin()+nbr_weights);
                biases.insert(biases.end(),best_controller.second.begin()+nbr_weights,best_controller.second.end());
                best_ctrl_gen.set_weights(weights);
                best_ctrl_gen.set_biases(biases);
                //update the archive
                if(use_ctrl_arch){
                    CartDesc morph_desc = learner.morph_genome.get_morph_desc();
                    controller_archive.update(std::make_shared<NNParamGenome>(best_ctrl_gen),1-best_controller.first,morph_desc.wheelNumber,morph_desc.jointNumber,morph_desc.sensorNumber);
                }
                //-

                //add new gene in gene_pool
                genome_t new_gene(learner.morph_genome,best_ctrl_gen,{fitness_fct(learner.ctrl_learner)});
                gene_pool.push_back(new_gene);
                //-

                //remove learner from learning pool and remove oldest gene
                remove_oldest_gene();
                increment_age();
                //-

                //Perform selection and generate a new morph gene.
                if(gene_pool.size() >= pop_size){
                    assert(gene_pool.size() == pop_size);
                    reproduction();
                    assert(learning_pool.size() == pop_size);
                }

            }else if(is_ctrl_next_gen){ //if the NIPES goes for a next gen
                init_new_ctrl_pop(learner);
            }
        }
        population.erase(population.begin() + corr_indexes[currentIndIndex]);
        population.shrink_to_fit();
        corr_indexes[currentIndIndex] = -1;
        for(int i = currentIndIndex; i < corr_indexes.size(); i++)
            corr_indexes[i]--;
    }

    return true;
}

void M_NIPES::reproduction(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    while(learning_pool.size() <= pop_size){ //create offspring until refilling entirely the learning pool
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
        }while(random_indexes.size() < 4);
        //-

        //Add these new gene to learning pool
        std::vector<genome_t> gene_subset;
        for(const int &idx: random_indexes)
            gene_subset.push_back(gene_pool[idx]);
        NN2CPPNGenome new_morph_gene = selection_fct(gene_subset);
        new_morph_gene.set_parameters(parameters);
        new_morph_gene.set_randNum(randomNum);
        learner_t new_learner(new_morph_gene);
        new_learner.ctrl_learner.set_parameters(parameters);
        learning_pool.push_back(new_learner);
        //-

        //Add it to the population with an empty ctrl genome to be build and determine its type.
        NN2CPPNGenome::Ptr morph_genome(new NN2CPPNGenome(new_morph_gene));
        EmptyGenome::Ptr ctrl_genome(new EmptyGenome);
        M_NIPESIndividual::Ptr ind(new M_NIPESIndividual(morph_genome,ctrl_genome));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
        int i = corr_indexes.size() - 1;
        while(corr_indexes[i] < 0) i--;
        corr_indexes.push_back(corr_indexes[i]+1);
        //-
    }
}

genome_t &M_NIPES::find_gene(int id){
    for(auto& gene: gene_pool)
        if(gene.morph_genome.id() == id)
            return gene;
    std::cerr << "Unable to find genome with id: " << id << std::endl;
}


learner_t& M_NIPES::find_learner(int id){
    for(auto& learner: learning_pool)
        if(learner.morph_genome.id() == id)
            return learner;
    std::cerr << "Unable to find learner with id: " << id << std::endl;
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
    int i = 1, oldest_idx = 0, oldest_age = gene_pool[0].age;
    for(; i < gene_pool.size(); i++){
        if(oldest_age < gene_pool[i].age){
            oldest_idx = i;
            oldest_age = gene_pool[i].age;
        }
    }
    gene_pool.erase(gene_pool.begin() + oldest_age);
}

void M_NIPES::increment_age(){
    for(auto& gene: gene_pool)
        gene.age++;
}

void M_NIPES::clean_learning_pool(){
    std::vector<int> elt_to_remove;
    int i = 0;
    for(const learner_t& l : learning_pool){
        if(l.ctrl_learner.is_initialized() && l.ctrl_learner.is_learning_finish())
            elt_to_remove.push_back(i);
        i++;
    }
    for(const int& i : elt_to_remove)
        learning_pool.erase(learning_pool.begin() + i);
}



void M_NIPES::init_new_learner(CMAESLearner &learner, const int wheel_nbr, int joint_nbr, int sensor_nbr){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    int nn_inputs = sensor_nbr*2;
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

    if(use_ctrl_arch){
        auto& starting_gen = controller_archive.archive[wheel_nbr][joint_nbr][sensor_nbr].first;

        if(starting_gen->get_weights().empty() && starting_gen->get_biases().empty())
            learner.init();
        else{
            std::vector<double> init_pt = std::dynamic_pointer_cast<NNParamGenome>(starting_gen)->get_full_genome();
            learner.init(init_pt);
        }
    }else learner.init();
}

void M_NIPES::init_new_ctrl_pop(learner_t &learner){
    auto new_ctrl_pop = learner.ctrl_learner.get_new_population();
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;

    for(const auto &wb : new_ctrl_pop){
        NN2CPPNGenome::Ptr morph_gen(new NN2CPPNGenome(learner.morph_genome));
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome(randomNum,parameters));
        ctrl_gen->set_weights(wb.first);
        ctrl_gen->set_biases(wb.second);
        ctrl_gen->set_nbr_hidden(nb_hidden);
        ctrl_gen->set_nbr_output(learner.morph_genome.get_morph_desc().wheelNumber + learner.morph_genome.get_morph_desc().jointNumber);
        ctrl_gen->set_nbr_input(learner.morph_genome.get_morph_desc().sensorNumber*2);
        ctrl_gen->set_nn_type(nn_type);
        Individual::Ptr ind(new M_NIPESIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_ctrl_archive(controller_archive);
        population.push_back(ind);
        int i = corr_indexes.size() - 1;
        while(corr_indexes[i] < 0) i--;
        corr_indexes.push_back(corr_indexes[i]+1);
    }
}
