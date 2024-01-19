#include "meim.hpp"

using namespace are;

void MEIMIndividual::createMorphology(){
    individual_id = morphGenome->id();
    morphology = std::make_shared<sim::Morphology_CPPNMatrix>(parameters);
    nn2_cppn_t cppn = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);

    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->set_morph_id(std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->id());

    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_position[0],init_position[1],init_position[2]);
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_cart_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getCartDesc());
    std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->set_organ_position_desc(std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosDesc());
}

void MEIMIndividual::createController(){
    int nb_joints = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_jointNumber();
    int nb_wheels = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_wheelNumber();
    int nb_sensors = 0;
    if(settings::getParameter<settings::Boolean>(parameters,"#withSensors").value)
        nb_sensors = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_sensorNumber();
    int nb_inputs = nb_sensors + nb_joints*2 + nb_wheels;
    int nb_outputs = nb_joints*2 + nb_wheels;
    control =  std::make_shared<hk::Homeokinesis>(nb_inputs,nb_outputs);
}

void MEIMIndividual::update(double delta_time){
    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);
    morphology->command(outputs);

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

    //set parameters and randNum to the genome as it is not contained in the serialisation
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}

void MEIM::setObjectives(size_t index, const std::vector<double> &objs){
    currentIndIndex = index;
    population[index]->setObjectives(objectives);
    newly_evaluated.push_back(index);
}

NN2CPPNGenome MEIM::best_of_subset(const std::vector<genome_t> gene_list){
    int obj_idx = settings::getParameter<settings::Integer>(gene_list[0].morph_genome.get_parameters(),"#morphSelectionObjective").value;

    double best_fitness = gene_list[0].objectives[obj_idx];
    int best_idx = 0;
    for(int i = 1; i < gene_list.size(); i++){
        if(best_fitness < gene_list[i].objectives[obj_idx]){
            best_fitness = gene_list[i].objectives[obj_idx];
            best_idx = i;
        }
    }
    nn2_cppn_t cppn = gene_list[best_idx].morph_genome.get_cppn();
    cppn.mutate();
    NN2CPPNGenome new_gene(cppn);
    new_gene.set_parents_ids({gene_list[best_idx].morph_genome.id(),-1});
    return new_gene;
}

void MEIM::reproduction(){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    int tournament_size = settings::getParameter<settings::Integer>(parameters,"#tournamentSize").value;

    while(population.size() < pop_size){
        //Random selection of indexes without duplicate
        std::vector<int> random_indexes;
        random_indexes.push_back(randomNum->randInt(0,parents_pool.size()-1));
        do{
            int rand_idx = randomNum->randInt(0,parents_pool.size()-1);
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
            gene_subset.push_back(parents_pool[idx]);
        NN2CPPNGenome new_morph_gene = best_of_subset(gene_subset);
        new_morph_gene.set_id(highest_morph_id++);
        new_morph_gene.set_parameters(parameters);
        new_morph_gene.set_randNum(randomNum);

        //Add it to the population
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

bool MEIM::update(const Environment::Ptr &env){
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    for(int index : newly_evaluated){
        Individual::Ptr ind = population[index];
        if((instance_type == settings::INSTANCE_SERVER && simulator_side) || instance_type == settings::INSTANCE_REGULAR){
            std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_final_position(env->get_final_position());
            if(env->get_name() == "obstacle_avoidance"){
                std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_visited_zones(std::dynamic_pointer_cast<sim::ObstacleAvoidance>(env)->get_visited_zone_matrix());
                std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_trajectories({env->get_trajectory()});
                std::dynamic_pointer_cast<MEIMIndividual>(ind)->set_descriptor_type(VISITED_ZONES);
            }else{
                std::cerr << "task unknown" << std::endl;
                exit(1);
            }
        }
        if((instance_type == settings::INSTANCE_SERVER && !simulator_side) || instance_type == settings::INSTANCE_REGULAR){

        }
        }

    }

}
