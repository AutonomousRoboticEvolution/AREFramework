#include "meim.hpp"

using namespace are;

void MEIMIndividual::createMorphology(){
    init_position = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    individual_id = morphGenome->id();
    morphology = std::make_shared<sim::Morphology_CPPNMatrix>(parameters);
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);

    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->set_morph_id(std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->id());

    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->createAtPosition(init_position[0],init_position[1],init_position[2]);
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_cart_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_organ_position_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosDesc());
}

void MEIMIndividual::createController(){
    nb_joints = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_jointNumber();
    nb_wheels = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_wheelNumber();
    nb_sensors = 0;
    if(settings::getParameter<settings::Boolean>(parameters,"#withSensors").value)
        nb_sensors = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_sensorNumber();
    int nb_inputs = nb_sensors + nb_joints + nb_wheels;
    int nb_outputs = nb_joints + nb_wheels;
    if(nb_outputs == 0)
        return;
    control =  std::make_shared<hk::Homeokinesis>(nb_inputs,nb_outputs);
    control->set_parameters(parameters);
    control->set_random_number(randNum);
}

void MEIMIndividual::update(double delta_time){
    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;
    double diff = delta_time/ctrl_freq - std::trunc(delta_time/ctrl_freq);
    double input_noise_lvl = settings::getParameter<settings::Double>(parameters,"#inputNoiseLevel").value;
    double output_noise_lvl = settings::getParameter<settings::Double>(parameters,"#outputNoiseLevel").value;
    if(control == nullptr)
        return;
    if( diff < 0.1){
        //- Retrieve sensors, joints and wheels values
        std::vector<double> inputs = morphology->update();
        std::vector<double> joints = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_joints_positions();
        for(double &j: joints)
            j = 2.*j/M_PI;
        std::vector<double> wheels = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_wheels_positions();
        inputs.insert(inputs.end(),joints.begin(),joints.end());
        inputs.insert(inputs.end(),wheels.begin(),wheels.end());
        //- add noise to the inputs
        for(double& v: inputs)
            v = randNum->normalDist(v,input_noise_lvl);
//        std::cout << "inputs: ";
//        for(const double& i : inputs)
//            std::cout << i << ";";
        //- get ouputs from the controller
        std::vector<double> outputs = control->update(inputs);
        //- add noise to the outputs
        for(double &o: outputs)
            o = randNum->normalDist(o,output_noise_lvl);
//        std::cout << "outputs: ";
//        for(const double& o : outputs)
//            std::cout << o << ";";
//        std::cout << std::endl;
        //- send command to the robot
        morphology->command(outputs);
    }
    //sim_time = delta_time;
    int morphHandle = std::dynamic_pointer_cast<sim::Morphology>(morphology)->getMainHandle();
    float position[3];
    simGetObjectPosition(morphHandle, -1, position);
}



std::string MEIMIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<MEIMIndividual>();
    oarch.register_type<NN2CPPNGenome>();
    oarch.register_type<EmptyGenome>();
    oarch << *this;
    return sstream.str();
}

void MEIMIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<MEIMIndividual>();
    iarch.register_type<NN2CPPNGenome>();
    iarch.register_type<EmptyGenome>();
    iarch >> *this;

    //- set parameters and randNum to the genome as it is not contained in the serialisation
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}


NN2CPPNGenome MEIM::best_of_subset(const std::vector<genome_t> gene_list){
   // int obj_idx = settings::getParameter<settings::Integer>(gene_list[0].morph_genome->get_parameters(),"#morphSelectionObjective").value;

    double best_fitness = gene_list[0].objectives[0];
    int best_idx = 0;
    for(int i = 1; i < gene_list.size(); i++){
        if(best_fitness < gene_list[i].objectives[0]){
            best_fitness = gene_list[i].objectives[0];
            best_idx = i;
        }
    }
    nn2_cppn_t cppn = gene_list[best_idx].morph_genome->get_cppn();
    cppn.mutate();
    NN2CPPNGenome new_gene(cppn);
    new_gene.set_parents_ids({gene_list[best_idx].morph_genome->id(),-1});
    return new_gene;
}

void MEIM::init(){
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    if(!simulator_side || instance_type == settings::INSTANCE_REGULAR){
        //- init random population of morphologies.
        int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        population.resize(pop_size);
        for(auto& ind: population){
            NN2CPPNGenome::Ptr morph_gen = std::make_shared<NN2CPPNGenome>();
            morph_gen->random();
            morph_gen->set_id(highest_morph_id++);
            Genome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();

            ind = std::make_shared<MEIMIndividual>(morph_gen,ctrl_gen);
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);

            morph_gen.reset();
            ctrl_gen.reset();
        }
    }else if(instance_type == settings::INSTANCE_SERVER && simulator_side){
        EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
        NN2CPPNGenome::Ptr morphgenome = std::make_shared<NN2CPPNGenome>(randomNum,parameters);
        MEIMIndividual::Ptr ind = std::make_shared<MEIMIndividual>(morphgenome,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void MEIM::init_next_pop(){

}


void MEIM::reproduction(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    int tournament_size = settings::getParameter<settings::Integer>(parameters,"#tournamentSize").value;

    while(population.size() < pop_size){
        //Random selection of indexes without duplicate
        std::vector<int> random_indexes;
        random_indexes.push_back(randomNum->randInt(0,parent_pool.size()-1));
        do{
            int rand_idx = randomNum->randInt(0,parent_pool.size()-1);
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

        //create a new morpholigical genome
        std::vector<genome_t> gene_subset;
        for(const int &idx: random_indexes)
            gene_subset.push_back(parent_pool[idx]);
        NN2CPPNGenome new_morph_gene = best_of_subset(gene_subset);
        new_morph_gene.set_id(highest_morph_id++);
        new_morph_gene.set_parameters(parameters);
        new_morph_gene.set_randNum(randomNum);

        //Add it to the population
        NN2CPPNGenome::Ptr morph_genome = std::make_shared<NN2CPPNGenome>(new_morph_gene);
        EmptyGenome::Ptr ctrl_genome = std::make_shared<EmptyGenome>();
        MEIMIndividual::Ptr ind = std::make_shared<MEIMIndividual>(morph_genome,ctrl_genome);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::vector<double> init_pos;
        init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
        std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_init_position(init_pos);
        population.push_back(ind);
        //-
    }
}

bool MEIM::update(const Environment::Ptr &env){
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    //    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if((instance_type == settings::INSTANCE_SERVER && simulator_side) || instance_type == settings::INSTANCE_REGULAR){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_final_position(env->get_final_position());
        if(env->get_name() == "obstacle_avoidance"){
            std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_visited_zones(std::dynamic_pointer_cast<sim::ObstacleAvoidance>(env)->get_visited_zone_matrix());
            std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_trajectory(env->get_trajectory());
        }else{
            std::cerr << "task unknown" << std::endl;
            exit(1);
        }
    }
    else if((instance_type == settings::INSTANCE_SERVER && !simulator_side) || instance_type == settings::INSTANCE_REGULAR){

        for(int &index : newly_evaluated){
            Individual::Ptr ind = population[index];

            //if indivdual's morphology has no actuactors, generate another morphology randomly and it back to the population
            int nb_joints = std::dynamic_pointer_cast<CPPNMorph>(ind->get_morphology())->get_jointNumber();
            int nb_wheels = std::dynamic_pointer_cast<CPPNMorph>(ind->get_morphology())->get_wheelNumber();
            if(nb_joints == 0 && nb_wheels == 0){
                NN2CPPNGenome::Ptr morph_gen = std::make_shared<NN2CPPNGenome>();
                morph_gen->random();
                morph_gen->set_id(highest_morph_id++);
                Genome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();

                Individual::Ptr new_ind = std::make_shared<MEIMIndividual>(morph_gen,ctrl_gen);
                new_ind->set_parameters(parameters);
                new_ind->set_randNum(randomNum);

                morph_gen.reset();
                ctrl_gen.reset();

                population.push_back(new_ind);
            }else{ // otherwise add this new individual to the parent pool and perform replacement
                //add new gene in gene_pool
                genome_t new_gene(std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome()),
                                  std::dynamic_pointer_cast<hk::Homeokinesis>(ind->get_control()),
                                  ind->getObjectives());
                //new_gene.trajectories = best_controller.trajectories;
                //misc::stdvect_to_eigenvect(best_controller.descriptor,new_gene.behavioral_descriptor);
                parent_pool.push_back(new_gene);
                new_genes.push_back(new_gene);
                //-
                remove_worst_parent();
            }
            population.erase(index);
        }

        newly_evaluated.clear();
        if(parent_pool.size() >= pop_size)
            reproduction();
    }
    return true;
}


bool MEIM::is_finish(){
    int max_nbr_eval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    if(numberEvaluation >= max_nbr_eval){
        return true;
    }
    return false;
}

bool MEIM::finish_eval(const Environment::Ptr & env){
    if(population[currentIndIndex]->get_control() == nullptr)
        return true;
    return false;
}

void MEIM::setObjectives(size_t index, const std::vector<double> &objs){
    //if(population[index]->get_control() == nullptr){
    //    currentIndIndex = index;
    //   population[index]->setObjectives({0});
    EA::setObjectives(index,objs);
    newly_evaluated.push_back(index);
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Individual " << population[index]->get_morph_genome()->id() << " evaluation finished with fitness of " << population[index]->getObjectives()[0] << std::endl;
}

void MEIM::remove_worst_parent(){
	int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
	if(parent_pool.size() <= pop_size)
		return;

	//- find lowest fitness value in the parent pool
	double lowest_obj = parent_pool[0].objectives[0];
	size_t worst_parent_idx = 0;
	for(size_t i = 1; i < parent_pool.size(); i++){
		if(lowest_obj > parent_pool[i].objectives[0]){
			lowest_obj = parent_pool[i].objectives[0];
			worst_parent_idx = i;
		}
	}
	//-
	
	//- then erase the worst one
	parent_pool.erase(parent_pool.begin() + worst_parent_idx);
	//-
}

void MEIM::fill_ind_to_eval(std::vector<int> &ind_to_eval){
	for(int i = 0; i < population.size(); i++)
		ind_to_eval.push_back(population.get_index(i));
}