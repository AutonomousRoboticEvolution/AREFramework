#include "meim.hpp"

using namespace are;

std::string act_obs_sample::to_string() const{
    std::stringstream sstr;
    sstr << observation[0];
    for(size_t i = 1; i < observation.size(); i++)
        sstr << "," << observation[i];
    sstr << ";" << next_action[0];
    for(size_t i = 1; i < next_action.size(); i++)
        sstr << "," << next_action[i];
    return sstr.str();
}

void MEIMIndividual::createMorphology(){
    int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;

    init_position = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    individual_id = morphGenome->id();
    if(genome_type == morph_genome_type::CPPN){
        morphology = std::make_shared<sim::CPPNMorphology>(parameters);
        nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
        std::dynamic_pointer_cast<sim::CPPNMorphology>(morphology)->set_cppn(cppn);
    }
    else if(genome_type == morph_genome_type::SQ_CPPN){
        morphology = std::make_shared<sim::SQCPPNMorphology>(parameters);
        sq_cppn::cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_cppn();
        sq_t quadric = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_quadric();
        std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->set_cppn(cppn);
        std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->set_quadric(quadric);
    }
    else if(genome_type == morph_genome_type::SQ_CG){
        morphology = std::make_shared<sim::SQMorphology>(parameters);
        cg_t comp_gen = std::dynamic_pointer_cast<SQGenome>(morphGenome)->get_components_genome();
        sq_t quadric = std::dynamic_pointer_cast<SQGenome>(morphGenome)->get_quadric();
        std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->set_comp_gen(comp_gen);
        std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->set_quadric(quadric);
    }else{
        std::cerr << "Unknown type of morphological genome" << std::endl;
        std::cerr << "Possible values for parameter #morphGenomeType" << std::endl;
        std::cerr << "1: CPPN | 2: SQ_CPPN | 3: SQ_CG" << std::endl;
        exit(1);
    }

    std::dynamic_pointer_cast<sim::Morphology>(morphology)->set_morph_id(morphGenome->id());
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->set_randNum(randNum);
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_position[0],init_position[1],init_position[2]);
    if(genome_type == morph_genome_type::CPPN){
        std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_feature_desc(std::dynamic_pointer_cast<sim::CPPNMorphology>(morphology)->getFeatureDesc());
        std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_organ_position_desc(std::dynamic_pointer_cast<sim::CPPNMorphology>(morphology)->getOrganPosDesc());
        std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_matrix_desc(std::dynamic_pointer_cast<sim::CPPNMorphology>(morphology)->getMatrixDesc());
    }else if(genome_type == morph_genome_type::SQ_CPPN){
        std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->set_feature_desc(std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->getFeatureDesc());
        std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->set_organ_position_desc(std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->getOrganPosDesc());
        std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->set_matrix_desc(std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->getMatrixDesc());
    }else if(genome_type == morph_genome_type::SQ_CG){
        cg_t cg = std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->get_comp_gen();
        std::dynamic_pointer_cast<SQGenome>(morphGenome)->set_components_genome(cg);
        std::dynamic_pointer_cast<SQGenome>(morphGenome)->set_feature_desc(std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->getFeatureDesc());
        std::dynamic_pointer_cast<SQGenome>(morphGenome)->set_organ_position_desc(std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->getOrganPosDesc());
        std::dynamic_pointer_cast<SQGenome>(morphGenome)->set_matrix_desc(std::dynamic_pointer_cast<sim::SQMorphology>(morphology)->getMatrixDesc());
    }else{
        std::cerr << "Unknown type of morphological genome" << std::endl;
        std::cerr << "Possible values for parameter #morphGenomeType" << std::endl;
        std::cerr << "1: CPPN | 2: SQ_CPPN | 3: SQ_CG" << std::endl;
        exit(1);
    }



}

void MEIMIndividual::createController(){
    bool use_fixed_control = settings::getParameter<settings::Boolean>(parameters,"#fixedController").value;
    nb_joints = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_jointHandles().size();
    nb_wheels = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_wheelHandles().size();
    nb_sensors = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_proxHandles().size();
    int nb_inputs = nb_sensors + nb_joints + nb_wheels;
    int nb_outputs = nb_joints + nb_wheels;
    if(nb_outputs == 0)
        return;
    if(use_fixed_control){
        control = std::make_shared<FixedController>();
        std::dynamic_pointer_cast<FixedController>(control)->init_nn(nb_inputs,6,nb_outputs,weight,bias); //initiate a Feedforward Neural Network with constant weights and biases
    }else{
        control =  std::make_shared<hk::Homeokinesis>(nb_inputs,nb_outputs);
        if (settings::getParameter<settings::Boolean>(parameters, "#add_noise").value)
            std::dynamic_pointer_cast<hk::Homeokinesis>(control)->add_noise(
                settings::getParameter<settings::Double>(parameters, "#noise_strength").value);
        std::dynamic_pointer_cast<hk::Homeokinesis>(control)->set_creativity(settings::getParameter<settings::Double>(parameters,"#creativity").value);
    }
    control->set_parameters(parameters);
    control->set_random_number(randNum);
    ctrl_time_counter = 0;

}

void MEIMIndividual::update(double delta_time){
    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;
    double time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    // double diff = delta_time/ctrl_freq - std::trunc(delta_time/ctrl_freq);
    double input_noise_lvl = settings::getParameter<settings::Double>(parameters,"#inputNoiseLevel").value;
    double output_noise_lvl = settings::getParameter<settings::Double>(parameters,"#outputNoiseLevel").value;
    if(control == nullptr)
        return;
    if( fabs(ctrl_time_counter - ctrl_freq) < 0.00001){
        ctrl_time_counter = 0;
        //- Retrieve sensors, joints and wheels values
        act_obs_sample aos;
        std::vector<double> inputs = morphology->update();
        assert(nb_sensors == inputs.size());
        std::vector<double> joints = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_joints_positions();
        assert(nb_joints == joints.size());
        for(double &j: joints)
            j = 2.*j/M_PI;
        std::vector<double> wheels = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_wheels_positions();
        for(double &w: wheels)
            w = w/M_PI;
        assert(wheels.size() == nb_wheels);
        inputs.insert(inputs.end(),joints.begin(),joints.end());
        inputs.insert(inputs.end(),wheels.begin(),wheels.end());
        aos.observation = inputs;
        //- add noise to the inputs
        for(double& v: inputs)
            v = randNum->normalDist(v,input_noise_lvl);
//        std::cout << "inputs: ";
//        for(const double& i : inputs)
//            std::cout << i << ";";
        //- get ouputs from the controller
        std::vector<double> outputs = control->update(inputs);
        aos.next_action = outputs;
        //- add noise to the outputs
        for(double &o: outputs)
            o = randNum->normalDist(o,output_noise_lvl);
//        std::cout << "outputs: ";
//        for(const double& o : outputs)
//            std::cout << o << ";";
//        std::cout << std::endl;
        //- send command to the robot
        morphology->command(outputs);
        rollout.push_back(aos);
    }
    ctrl_time_counter += time_step;
    //sim_time = delta_time;
    // int morphHandle = std::dynamic_pointer_cast<sim::Morphology>(morphology)->getMainHandle();
    // double position[3];
    // simGetObjectPosition(morphHandle, -1, position);
}



std::string MEIMIndividual::to_string() const
{
    int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;

    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<MEIMIndividual>();
    if(genome_type == morph_genome_type::CPPN)
        oarch.register_type<NN2CPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CPPN)
        oarch.register_type<SQCPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CG)
        oarch.register_type<SQGenome>();

    oarch.register_type<EmptyGenome>();
    oarch << *this;
    return sstream.str();
}

void MEIMIndividual::from_string(const std::string &str){
    int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;

    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<MEIMIndividual>();
    if(genome_type == morph_genome_type::CPPN)
        iarch.register_type<NN2CPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CPPN)
        iarch.register_type<SQCPPNGenome>();
    else if(genome_type == morph_genome_type::SQ_CG)
        iarch.register_type<SQGenome>();
    iarch.register_type<EmptyGenome>();
    iarch >> *this;

    //- set parameters and randNum to the genome as it is not contained in the serialisation
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}


MEIM::MEIM(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param):
    EA(rn,param){
    comp_mut_params::_position_mutation_rate =
        settings::getParameter<settings::Double>(parameters,"#compPositionMutRate").value;
    comp_mut_params::_type_mutation_rate =
        settings::getParameter<settings::Double>(parameters,"#compTypeMutRate").value;
    comp_mut_params::_modify_comp_list_mutation_rate =
        settings::getParameter<settings::Double>(parameters,"#compListMutRate").value;
    comp_mut_params::_add_remove_comp_prob =
        settings::getParameter<settings::Double>(parameters,"#addRemoveCompProb").value;
    quadric_mut_params::_mutation_rate =
        settings::getParameter<settings::Double>(parameters,"#quadricsParamMutRate").value;
    quadric_mut_params::_sigma =
        settings::getParameter<settings::Double>(parameters,"#quadricsSigma").value;
    quadric_mut_params::_symmetry_mutation_rate =
        settings::getParameter<settings::Double>(parameters,"#quadricsSymmetryMutRate").value;

        
}

Genome::Ptr MEIM::best_of_subset(const std::vector<genome_t> gene_list){
   // int obj_idx = settings::getParameter<settings::Integer>(gene_list[0].morph_genome->get_parameters(),"#morphSelectionObjective").value;

    double best_fitness = gene_list[0].objectives[0];
    int best_idx = 0;
    for(size_t i = 1; i < gene_list.size(); i++){
        if(best_fitness < gene_list[i].objectives[0]){
            best_fitness = gene_list[i].objectives[0];
            best_idx = i;
        }
    }
    Genome::Ptr new_gene = gene_list[best_idx].morph_genome->clone();
    new_gene->mutate();
    new_gene->set_parents_ids({gene_list[best_idx].morph_genome->id(),-1});
    return new_gene;
}

void MEIM::init(){
    nn2::rgen_t::gen.seed(randomNum->getSeed());
    int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;

    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    if(!simulator_side || instance_type == settings::INSTANCE_REGULAR){
        bool use_fixed_control = settings::getParameter<settings::Boolean>(parameters,"#fixedController").value;
        if(use_fixed_control){
            weight = randomNum->randDouble(-0.2,0.2);
            bias = randomNum->randDouble(-0.2,0.2);
            parameters->emplace("#weight",std::make_shared<settings::Double>(weight));
            parameters->emplace("#bias",std::make_shared<settings::Double>(bias));
        }
        //- init random population of morphologies.
        int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        population.resize(pop_size);
        for(auto& ind: population){
            Genome::Ptr morph_gen;
            if(genome_type == morph_genome_type::CPPN)
                morph_gen = std::make_shared<NN2CPPNGenome>(randomNum,parameters);
            else if(genome_type == morph_genome_type::SQ_CPPN)
                morph_gen = std::make_shared<SQCPPNGenome>(randomNum,parameters);
            else if(genome_type == morph_genome_type::SQ_CG)
                morph_gen = std::make_shared<SQGenome>(randomNum,parameters);
            else{
                std::cerr << "Unknown type of morphological genome" << std::endl;
                std::cerr << "Possible values for parameter #morphGenomeType" << std::endl;
                std::cerr << "1: CPPN | 2: SQ_CPPN | 3: SQ_CG" << std::endl;
                exit(1);
            }
            morph_gen->random();
            morph_gen->set_id(highest_morph_id++);
            EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();

            ind = std::make_shared<MEIMIndividual>(morph_gen,ctrl_gen);
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);

            if(use_fixed_control)
                std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_weight_bias(weight,bias);


            morph_gen.reset();
            ctrl_gen.reset();
        }
    }else if(instance_type == settings::INSTANCE_SERVER && simulator_side){
        EmptyGenome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();
        Genome::Ptr morph_gen;
        if(genome_type == morph_genome_type::CPPN)
            morph_gen = std::make_shared<NN2CPPNGenome>(randomNum,parameters);
        else if(genome_type == morph_genome_type::SQ_CPPN)
            morph_gen = std::make_shared<SQCPPNGenome>(randomNum,parameters);
        else if(genome_type == morph_genome_type::SQ_CG)
            morph_gen = std::make_shared<SQGenome>(randomNum,parameters);
        else{
            std::cerr << "Unknown type of morphological genome" << std::endl;
            std::cerr << "Possible values for parameter #morphGenomeType" << std::endl;
            std::cerr << "1: CPPN | 2: SQ_CPPN | 3: SQ_CG" << std::endl;
            exit(1);
        }
        MEIMIndividual::Ptr ind = std::make_shared<MEIMIndividual>(morph_gen,ctrl_gen);
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
    bool use_fixed_control = settings::getParameter<settings::Boolean>(parameters,"#fixedController").value;

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
        Genome::Ptr new_morph_gene = best_of_subset(gene_subset);
        new_morph_gene->set_id(highest_morph_id++);
        new_morph_gene->set_parameters(parameters);
        new_morph_gene->set_randNum(randomNum);

        //Add it to the population
        EmptyGenome::Ptr ctrl_genome = std::make_shared<EmptyGenome>();
        MEIMIndividual::Ptr ind = std::make_shared<MEIMIndividual>(new_morph_gene,ctrl_genome);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        if(use_fixed_control)
            std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_weight_bias(weight,bias);
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
        std::dynamic_pointer_cast<MEIMIndividual>(ind)->reset_control();
    }
    if((instance_type == settings::INSTANCE_SERVER && !simulator_side) || instance_type == settings::INSTANCE_REGULAR){
        int genome_type = settings::getParameter<settings::Integer>(parameters,"#morphGenomeType").value;

        for(int &index : newly_evaluated){
            std::cout << "update for individual indexed " << index << std::endl;
            Individual::Ptr ind = population[index];

            //if indivdual's morphology has no actuactors, generate another morphology randomly and it back to the population
            int nb_joints = std::dynamic_pointer_cast<MEIMIndividual>(ind)->get_nb_joints();
            int nb_wheels = std::dynamic_pointer_cast<MEIMIndividual>(ind)->get_nb_wheels();
            if(nb_joints == 0 && nb_wheels == 0){
                Genome::Ptr morph_gen;
                if(genome_type == morph_genome_type::CPPN)
                    morph_gen = std::make_shared<NN2CPPNGenome>(randomNum,parameters);
                else if(genome_type == morph_genome_type::SQ_CPPN)
                    morph_gen = std::make_shared<SQCPPNGenome>(randomNum,parameters);
                else if(genome_type == morph_genome_type::SQ_CG)
                    morph_gen = std::make_shared<SQGenome>(randomNum,parameters);
                else{
                    std::cerr << "Unknown type of morphological genome" << std::endl;
                    std::cerr << "Possible values for parameter #morphGenomeType" << std::endl;
                    std::cerr << "1: CPPN | 2: SQ_CPPN | 3: SQ_CG" << std::endl;
                    exit(1);
                }
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
                numberEvaluation++;
                //add new gene in gene_pool                
                genome_t new_gene(ind->get_morph_genome(),
                                  std::dynamic_pointer_cast<hk::Homeokinesis>(ind->get_control()),
                                  ind->getObjectives());
                new_gene.trajectory = std::dynamic_pointer_cast<MEIMIndividual>(ind)->get_trajectory();
                new_gene.rollout = std::dynamic_pointer_cast<MEIMIndividual>(ind)->get_rollout();
                //new_gene.trajectories = best_controller.trajectories;
                //misc::stdvect_to_eigenvect(best_controller.descriptor,new_gene.behavioral_descriptor);
                parent_pool.push_back(new_gene);
                new_genes.push_back(new_gene);
                //-
                remove_worst_parent();
                if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
                    std::cout << numberEvaluation << " evaluations over " << settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value << std::endl;
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
    std::cout << "set objectives of individual indexed " << index << std::endl;
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
    for(size_t i = 0; i < population.size(); i++)
		ind_to_eval.push_back(population.get_index(i));
}
