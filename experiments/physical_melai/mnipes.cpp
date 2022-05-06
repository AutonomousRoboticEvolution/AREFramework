#include "mnipes.hpp"

using namespace are;

void PMEIndividual::createMorphology(){
    morphology.reset(new sim::Morphology_CPPNMatrix(parameters));
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(0,0,0.12);
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_cart_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_organ_position_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosDesc());

    listOrganTypes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganTypes();
    listOrganPos = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosList();
    listOrganOri = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganOriList();
    skeletonListIndices = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getSkeletonListIndices();
    skeletonListVertices = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getSkeletonListVertices();
}

MNIPES::MNIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){
    settings::defaults::parameters->emplace("#tournamentSize",new settings::Integer(4));
    settings::defaults::parameters->emplace("#nbrOfOffSprings",new settings::Integer(1));
}

void MNIPES::init(){


}

void MNIPES::init_random_pop(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    for(int i = 0; i < pop_size; i++){
        //create a new morph genome with random structure and parameters
        NN2CPPNGenome new_morph_gene;
        new_morph_gene.random();
        new_morph_gene.set_parameters(parameters);
        new_morph_gene.set_randNum(randomNum);
        //-

        //Add it to the population with an empty ctrl genome to be submitted to manufacturability test.
        NN2CPPNGenome::Ptr morph_genome(new NN2CPPNGenome(new_morph_gene));
        EmptyGenome::Ptr ctrl_genome(new EmptyGenome);
        PMEIndividual::Ptr ind(new PMEIndividual(morph_genome,ctrl_genome));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void MNIPES::init_next_pop(){
    if(morph_genomes_info.empty())
        init_random_pop();
    else
        _reproduction();
}


bool MNIPES::update(const Environment::Ptr &env){
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    PMEIndividual::Ptr ind(new PMEIndividual);
    objectives = env->fitnessFunction(ind);
    ind.reset();

    std::vector<double> final_pos = env->get_final_position();
    Eigen::VectorXd desc(3);
    desc << (final_pos[0]+arena_size/2.)/arena_size,
            (final_pos[1]+arena_size/2.)/arena_size,
            (final_pos[2]+arena_size/2.)/arena_size;
    trajectory = env->get_trajectory();


    //update learner
    auto& learner = learners[currentIndIndex];
    numberEvaluation++;
    learner.update_pop_info(objectives,desc);
    learner.step();

    if(learner.is_learning_finish()){//learning is finished for this body plan
        if(verbose)
            std::cout << "Learning is finished for robot " << currentIndIndex << std::endl;
        //Update Controller Archive
        std::vector<double> weights;
        std::vector<double> biases;
        int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
        int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#nbrHiddenNeurons").value;
        auto &best_controller = learner.get_best_solution();
        int nbr_weights = learner.get_nbr_weights();
        weights.insert(weights.end(),best_controller.second.begin(),best_controller.second.begin()+nbr_weights);
        biases.insert(biases.end(),best_controller.second.begin()+nbr_weights,best_controller.second.end());
        best_current_ctrl_genome.set_weights(weights);
        best_current_ctrl_genome.set_biases(biases);
        best_current_ctrl_genome.set_nbr_input(learner.get_nbr_inputs());
        best_current_ctrl_genome.set_nbr_output(learner.get_nbr_outputs());
        best_current_ctrl_genome.set_nbr_hidden(nb_hidden);
        best_current_ctrl_genome.set_nn_type(nn_type);
        //update the archive
        if(use_ctrl_arch){
            std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
            std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
            int wheels,joints,sensors;
            ioh::load_nbr_organs(repository + "/" + exp_name,currentIndIndex,wheels,joints,sensors);
            ctrl_archive.update(std::make_shared<NNParamGenome>(best_current_ctrl_genome),1-best_controller.first,wheels,joints,sensors);
        }
        //-
    }
    return true;
}

void MNIPES::_survival(const ioh::MorphGenomeInfoMap &morph_gen_info, std::vector<int> &list_ids){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

    //if the population size is greater or equal to the number of genomes currently available (in morph_gen_info), skip the survival step
    if(morph_gen_info.size() <= pop_size){
        for(const auto& elt: morph_gen_info)
            list_ids.push_back(elt.first);
        return;
    }

    int max_gen = 0;
    std::multimap<int,int> map_per_gen;
    for(const auto& elt: morph_gen_info){
        int gen = settings::cast<settings::Integer>(elt.second.at("generation"))->value;
        if(max_gen < gen) max_gen = gen;
        map_per_gen.emplace(gen,elt.first);
    }

    std::vector<int> tmp;
    int current_gen = max_gen;
    while(list_ids.size() == pop_size){
        //retrieve all the morph ids of the last gen i.e. youngest
        auto range = map_per_gen.equal_range(current_gen);
        for(auto it = range.first; it != range.second; it++)
            tmp.push_back(it->second);

        //if the individual of the current age does not fill the population
        if(tmp.size() < pop_size - list_ids.size()){
            list_ids.insert(list_ids.end(),tmp.begin(),tmp.end());
            current_gen--;
        }
        else if(tmp.size() > pop_size - list_ids.size()){

            //shuffle
            std::vector<int> res;
            std::vector<int> idxes;
            for(size_t i = 0; i < tmp.size(); i++)
                idxes.push_back(i);

            for(size_t i = 0; i < pop_size; i++){
                int n = randomNum->randInt(0,idxes.size()-1);
                res.push_back(tmp[idxes[n]]);
                idxes.erase(idxes.begin() + n);
                idxes.shrink_to_fit();
            }

            list_ids.insert(list_ids.end(),res.begin(),res.end());
        }
    }
}

void MNIPES::_reproduction(){
    int nbr_of_offsprings = settings::getParameter<settings::Integer>(parameters,"#nbrOfOffsprings").value;
    int tournament_size = settings::getParameter<settings::Integer>(parameters,"#tournamentSize").value;

    std::vector<int> genome_ids;
    for(const auto &elt: morph_genomes)
        genome_ids.push_back(elt.first);

    auto random_selection = [&](std::vector<int> ids, int size) -> std::vector<int>{
            std::vector<int> random_indexes;
        if(ids.size() < size){
            for(int i = 0; i < ids.size();i++)
            random_indexes.push_back(i);
        }else{
            //Selection of tournament size random parents
            random_indexes.push_back(randomNum->randInt(0,ids.size() - 1));
            do{
                int rand_idx = randomNum->randInt(0,ids.size() - 1);
                bool already_drawn = false;
                for(const int& idx: random_indexes)
                    if(idx == rand_idx){
                        already_drawn = true;
                        break;
                    }
                if(!already_drawn)
                    random_indexes.push_back(rand_idx);
            }while(random_indexes.size() < size); //select up to 4 random individuals.
        }
        return random_indexes;
    };

    auto best_of_subset = [&](const std::vector<int> &ri) -> int{ //Selection of best parents among the subset of 4
            double best_fitness = 0;
            int best_id = 0;
            for(const int &i: ri){
                double fit = settings::cast<settings::Double>(morph_genomes_info[genome_ids[i]]["fitness"])->value;
                if(best_fitness < fit){
                    best_fitness = fit;
                    best_id = genome_ids[i];
                }
            }
            return best_id;
    };


    for(int i = 0; i < nbr_of_offsprings; i++){
        NN2CPPNGenome new_morph_gene;
        /** CROSSBREEDING **\
         * 1 - Check if crossbreeding should be activated
         * 2 - Select individual from robot library to be crossbreed
         * 3 - pairing
         */
        Crossbreeding crossbreeding(parameters,randomNum);
        if(crossbreeding.should_crossbreed(morph_genomes_info)){
            std::map<int,NN2CPPNGenome> robot_lib_genomes;
            crossbreeding.selection(tournament_size,robot_lib_genomes);
            std::vector<int> rl_genome_ids;
            for(const auto &elt: robot_lib_genomes)
                rl_genome_ids.push_back(elt.first);
            std::vector<int> ri_genomes = random_selection(genome_ids,tournament_size);
            int rl_id = rl_genome_ids[random_selection(rl_genome_ids,1).back()];
            int best_id = best_of_subset(ri_genomes);
            nn2_cppn_t rl_parent = robot_lib_genomes[rl_id].get_cppn();
            nn2_cppn_t parent = morph_genomes[best_id].get_cppn();
            nn2_cppn_t new_cppn;
            rl_parent.crossover(parent,new_cppn);

            new_morph_gene = morph_genomes[best_id];
            new_morph_gene.set_cppn(new_cppn);
           // new_morph_gene.mutate(); //mutate?
            new_morph_gene.incr_generation();
            new_morph_gene.set_parameters(parameters);
            new_morph_gene.set_randNum(randomNum);
        }
        else{//without crossbreeding
            std::vector<int> random_indexes = random_selection(genome_ids,tournament_size);
            int best_id = best_of_subset(random_indexes);
            new_morph_gene = morph_genomes[best_id];
            new_morph_gene.mutate();
            new_morph_gene.incr_generation();
            new_morph_gene.set_parameters(parameters);
            new_morph_gene.set_randNum(randomNum);
        }

        //Add it to the population with an empty ctrl genome to be submitted to manufacturability test.
        NN2CPPNGenome::Ptr morph_genome(new NN2CPPNGenome(new_morph_gene));
        EmptyGenome::Ptr ctrl_genome(new EmptyGenome);
        PMEIndividual::Ptr ind(new PMEIndividual(morph_genome,ctrl_genome));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
        //-
    }
}

void MNIPES::init_learner(int id){
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#nbrHiddenNeurons").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    bool load_existing_ctrls = settings::getParameter<settings::Boolean>(parameters,"#loadExistingControllers").value;
    bool useArucoAsInput = settings::getParameter<settings::Boolean>(parameters,"#useArucoAsInput").value;
    std::string learner_file = settings::getParameter<settings::String>(parameters,"#learnerToLoad").value;


    int wheels, joints, sensors;
    ioh::load_nbr_organs(repository + "/" + exp_name,currentIndIndex,wheels,joints,sensors);
    int nn_inputs;
    if (useArucoAsInput){
        nn_inputs = sensors*2 + 1;
    }else{
        nn_inputs = sensors*2;
    }
    int nn_outputs = wheels + joints;
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
    CMAESLearner learner(nbr_weights,nbr_bias,nn_inputs,nn_outputs);
    learner.set_parameters(parameters);
    learner.set_randNum(randomNum);

    //Load an existing learner
    if(boost::filesystem::exists(Logging::log_folder + "/" + learner_file)){
        learner.init();
        learner.from_file(Logging::log_folder + "/" + learner_file);
        std::cout << learner.print_info() << std::endl;
    }
    else{
        if(!load_existing_ctrls && !use_ctrl_arch)
            learner.init();
        else{
            NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
            if(load_existing_ctrls)// load existing controllers
                ioh::load_controller_genome(repository + "/" + exp_name,currentIndIndex,ctrl_gen);
            else if(use_ctrl_arch)// load from controller archive
                ctrl_gen = ctrl_archive.archive[wheels][joints][sensors].first;
            else learner.init();

            if(ctrl_gen->get_weights().empty() && ctrl_gen->get_biases().empty())
                learner.init();
            else learner.init(ctrl_gen->get_full_genome());
        }
    }

    learners.emplace(id,learner);
}

const Genome::Ptr MNIPES::get_next_controller_genome(int id){
    population.clear();
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#nbrHiddenNeurons").value;

    //** Create Controller to send to robot
    NN2Control<elman_t>::Ptr ctrl(new NN2Control<elman_t>);
    auto nn_params = learners[id].update_ctrl(ctrl);
    NNParamGenome::Ptr gen(new NNParamGenome(randomNum,parameters));
    gen->set_weights(nn_params.first);
    gen->set_biases(nn_params.second);
    gen->set_nbr_hidden(nb_hidden);
    gen->set_nn_type(nn_type);
    gen->set_nbr_input(learners[id].get_nbr_inputs());
    gen->set_nbr_output(learners[id].get_nbr_outputs());
    //*/
    //** Create individual only for logging
    EmptyGenome::Ptr empty_gen(new EmptyGenome);
    PMEIndividual::Ptr ind(new PMEIndividual(empty_gen,gen));
    ind->set_parameters(parameters);
    ind->set_randNum(randomNum);
    population.push_back(ind);
    //*/

    return gen;
}

void MNIPES::load_data_for_generate(){
    std::string repo = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;


    ioh::MorphGenomeInfoMap morph_gen_info;
    std::string exp_folder = repo + std::string("/") + exp_name;
    ioh::load_morph_genomes_info(exp_folder,morph_gen_info);
    //if morph genomes info is empty, then we are in a initialization situation.
    if(morph_gen_info.empty())
        return;

    std::vector<int> list_to_load;
    _survival(morph_gen_info,list_to_load);

    /** CrossMigration
     *
     */

    ioh::load_morph_genomes<NN2CPPNGenome>(exp_folder,list_to_load,morph_genomes);

}

void MNIPES::write_data_for_generate(){
    std::string repo = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    ioh::write_morph_blueprints<PMEIndividual>(repo + "/" + exp_name,population);
    ioh::write_morph_meshes<PMEIndividual>(repo + "/" + exp_name,population);
    ioh::write_morph_genomes(repo + "/" + exp_name,population);
}

void MNIPES::load_data_for_update() {
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;

    //load controller archive
    if(use_ctrl_arch){
        int max_nbr_organs = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;
        ctrl_archive.init(max_nbr_organs,max_nbr_organs,max_nbr_organs);
        ctrl_archive.from_file(repository + "/" + exp_name + "/logs/controller_archive");
    }
    //create a first genome controller for the first robot chosen
    init_learner(currentIndIndex);
}

void MNIPES::write_data_for_update(){
    //Go through the list of learners and fill the GP with the one for whom learning is finished
    for(const auto &learner: learners){
        if(!learner.second.is_learning_finish())
            continue;
        std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
        std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;

        std::map<int,NN2CPPNGenome> gen;
        ioh::load_morph_genomes<NN2CPPNGenome>(repository + "/" + exp_name + "/waiting_to_be_evaluated/",{learner.first},gen);
        ioh::MorphGenomeInfo morph_info;
        morph_info.emplace("generation",new settings::Integer(gen[learner.first].get_generation()));
        morph_info.emplace("fitness",new settings::Float(1-learner.second.get_best_solution().first));
        ioh::add_morph_genome_to_gp(repository + "/" + exp_name,learner.first,morph_info);
    }
}

void MNIPES::write_morph_descriptors(){
    for(const auto& ind : population){

    }
}
