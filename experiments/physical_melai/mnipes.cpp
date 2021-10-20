#include "mnipes.hpp"

using namespace are;

void PMEIndividual::createMorphology(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
    morphology.reset(new sim::Morphology_CPPNMatrix(parameters));
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->setNN2CPPN(cppn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_x,init_y,0.15);
    cppn = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getNN2CPPN();
    morphDesc = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getMorphDesc();
    testRes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getRobotManRes();
    listOrganTypes = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganTypes();
    listOrganPos = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganPosList();
    listOrganOri = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getOrganOriList();
    skeletonListIndices = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getSkeletonListIndices();
    skeletonListVertices = std::dynamic_pointer_cast<sim::Morphology_CPPNMatrix>(morphology)->getSkeletonListVertices();
}


void MNIPES::init(){


}

void MNIPES::init_next_pop(){

    _reproduction();

//    CPPNGenome::Ptr morph_gen;
//    EmptyGenome::Ptr ctrl_gen;
//    //load morphology genome
//    for(size_t i = 0; i < morph_gen_files.size(); i++){
//        NEAT::Genome neat_gen(morph_gen_files[i].c_str());
//        morph_gen.reset(new CPPNGenome(neat_gen));
//        morph_gen->set_randNum(randomNum);
//        morph_gen->set_parameters(parameters);

//        ctrl_gen.reset(new EmptyGenome);

//        Individual::Ptr ind(new PMEIndividual(morph_gen,ctrl_gen));
//        ind->set_parameters(parameters);
//        ind->set_randNum(randomNum);
//        population.push_back(ind);
//    }
//    ctrl_gen.reset();
//    morph_gen.reset();
}

void MNIPES::setObjectives(size_t current_id, const std::vector<double> &objs){

}

bool MNIPES::update(const Environment::Ptr &){
    bool use_ctrl_arch = settings::getParameter<settings::Boolean>(parameters,"#useControllerArchive").value;
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

//    clean_learning_pool();

//    std::dynamic_pointer_cast<PMEIndividual>(ind)->set_final_position(env->get_final_position());
//    std::dynamic_pointer_cast<PMEIndividual>(ind)->set_trajectory(env->get_trajectory());;

    //If one the client or just sequential mode

//    int morph_id = std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->id();
//    learner_t &learner = find_learner(morph_id);
//    if(ind->get_ctrl_genome()->get_type() == "empty_genome"){//if ctrl genome is empty
//        learner.morph_genome.set_morph_desc(std::dynamic_pointer_cast<NN2CPPNGenome>(ind->get_morph_genome())->get_morph_desc());
//        int wheel_nbr = learner.morph_genome.get_morph_desc().wheelNumber;
//        int joint_nbr = learner.morph_genome.get_morph_desc().jointNumber;
//        int sensor_nbr = learner.morph_genome.get_morph_desc().sensorNumber;
//        if(wheel_nbr > 0 || joint_nbr > 0){
//            init_new_learner(learner.ctrl_learner,wheel_nbr,joint_nbr,sensor_nbr);
//            init_new_ctrl_pop(learner);
//        }else{
//            genome_t new_gene(learner.morph_genome,NNParamGenome(),{0});
//            gene_pool.push_back(new_gene);
//            learner.ctrl_learner.to_be_erased();
//        }
//    }else{
//        numberEvaluation++;
//        //update learner
//        learner.ctrl_learner.update_pop_info(ind->getObjectives(),ind->descriptor());
//        bool is_ctrl_next_gen = learner.ctrl_learner.step();
//        //-

//        if(learner.ctrl_learner.is_learning_finish()){//learning is finished for this body plan

//            //Update Controller Archive
//            std::vector<double> weights;
//            std::vector<double> biases;
//            NNParamGenome best_ctrl_gen;
//            auto &best_controller = learner.ctrl_learner.get_best_solution();
//            int nbr_weights = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_weights().size();
//            weights.insert(weights.begin(),best_controller.second.begin(),best_controller.second.begin()+nbr_weights);
//            biases.insert(biases.begin(),best_controller.second.begin()+nbr_weights,best_controller.second.end());
//            best_ctrl_gen.set_weights(weights);
//            best_ctrl_gen.set_biases(biases);
//            //update the archive
//            if(use_ctrl_arch){
//                CartDesc morph_desc = learner.morph_genome.get_morph_desc();
//                controller_archive.update(std::make_shared<NNParamGenome>(best_ctrl_gen),1-best_controller.first,morph_desc.wheelNumber,morph_desc.jointNumber,morph_desc.sensorNumber);
//            }
//            //-

//            //add new gene in gene_pool
//            genome_t new_gene(learner.morph_genome,best_ctrl_gen,{fitness_fct(learner.ctrl_learner)});
//            gene_pool.push_back(new_gene);
//            //-

//            //remove learner from learning pool and remove oldest gene
//            remove_oldest_gene();
//            increment_age();
//            //-

//            //Perform selection and generate a new morph gene.
//            if(gene_pool.size() >= pop_size){
//                assert(gene_pool.size() == pop_size);
//                reproduction();
//                assert(learning_pool.size() == pop_size);
//            }

//        }else if(is_ctrl_next_gen){ //if the NIPES goes for a next gen
//            init_new_ctrl_pop(learner);
//        }
//    }


    return true;
}

void MNIPES::_survival(const phy::MorphGenomeInfoMap &morph_gen_info, std::vector<int> &list_ids){
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;

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

void MNIPES::load_data_for_generate(){
    std::string repo = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    phy::MorphGenomeInfoMap morph_gen_info;
    std::string exp_folder = repo + std::string("/") + exp_name;
    phy::load_morph_genomes_info(exp_folder,morph_gen_info);
    std::vector<int> list_to_load;
    _survival(morph_gen_info,list_to_load);
    phy::load_morph_genomes<NN2CPPNGenome>(exp_folder,morph_gen_info,list_to_load,morph_genomes);

}

void MNIPES::write_data_for_generate(){

    const auto& ind = population[currentIndIndex];
    const auto& id = ids[currentIndIndex];
    std::stringstream sst;
    sst << "morph_desc_" << currentIndIndex;
    std::ofstream ofs(Logging::log_folder + std::string("/waiting_to_be_built/")  + sst.str() , std::ios::out | std::ios::ate | std::ios::app);

    if(!ofs)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/waiting_to_be_built/")  + sst.str() << std::endl;
        return;
    }

    for(int j = 0; j < std::dynamic_pointer_cast<PMEIndividual>(ind)->get_morphDesc().cols(); j++)
        ofs << std::dynamic_pointer_cast<PMEIndividual>(ind)->get_morphDesc()(j) << ";";

    // Export blueprint

    std::stringstream sst_blueprint;
    sst_blueprint << "blueprint_" << id.first << "_" << id.second << ".csv";
    std::ofstream ofs_blueprint(Logging::log_folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str() , std::ios::out | std::ios::ate | std::ios::app);
    if(!ofs)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str() << std::endl;
        return;
    }
    std::vector<int> tempOrganTypes = std::dynamic_pointer_cast<PMEIndividual>(ind)->getListOrganTypes();
    std::vector<std::vector<float>> tempOrganPos = std::dynamic_pointer_cast<PMEIndividual>(ind)->getListOrganPos();
    std::vector<std::vector<float>> tempOrganOri = std::dynamic_pointer_cast<PMEIndividual>(ind)->getListOrganOri();
    for (int i = 0; i < tempOrganTypes.size(); i++) {
        ofs_blueprint << "0" << "," << tempOrganTypes.at(i) << ","
                      << tempOrganPos.at(i).at(0) << "," << tempOrganPos.at(i).at(1) << ","
                      << tempOrganPos.at(i).at(2) << ","
                      << tempOrganOri.at(i).at(0) << "," << tempOrganOri.at(i).at(1) << ","
                      << tempOrganOri.at(i).at(2) << ","
                      << std::endl;
    }

    // Export mesh file

    const auto **verticesMesh = new const simFloat *[2];
    const auto **indicesMesh = new const simInt *[2];
    auto *verticesSizesMesh = new simInt[2];
    auto *indicesSizesMesh = new simInt[2];
    verticesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListVertices().data();
    verticesSizesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListVertices().size();
    indicesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListIndices().data();
    indicesSizesMesh[0] = std::dynamic_pointer_cast<PMEIndividual>(ind)->getSkeletonListIndices().size();

    std::stringstream filepath;
    filepath << Logging::log_folder << "/waiting_to_be_built/mesh_" << id.first << "_" << id.second << ".stl";

    //fileformat: the fileformat to export to:
    //  0: OBJ format, 3: TEXT STL format, 4: BINARY STL format, 5: COLLADA format, 6: TEXT PLY format, 7: BINARY PLY format
    simExportMesh(3, filepath.str().c_str(), 0, 1.0f, 1, verticesMesh, verticesSizesMesh, indicesMesh, indicesSizesMesh, nullptr, nullptr);

    delete[] verticesMesh;
    delete[] verticesSizesMesh;
    delete[] indicesMesh;
    delete[] indicesSizesMesh;
}

void MNIPES::load_data_for_update() {

}

void MNIPES::write_data_for_update(){

}
