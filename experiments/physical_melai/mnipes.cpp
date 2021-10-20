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

    //If on the client or just sequential mode
    //update learner
    auto& learner = learners[currentIndIndex];
    numberEvaluation++;
    learner.update_pop_info(ind->getObjectives(),ind->descriptor());
    learner.step();

    if(learner.is_learning_finish()){//learning is finished for this body plan
        //Update Controller Archive
        std::vector<double> weights;
        std::vector<double> biases;
        NNParamGenome best_ctrl_gen;
        auto &best_controller = learner.get_best_solution();
        int nbr_weights = learner.get_nbr_weights();
        weights.insert(weights.begin(),best_controller.second.begin(),best_controller.second.begin()+nbr_weights);
        biases.insert(biases.begin(),best_controller.second.begin()+nbr_weights,best_controller.second.end());
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
    }
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

void MNIPES::init_learner(int id){
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#nbrHiddenNeurons").value;

    int wheels, joints, sensors;
    phy::load_nbr_organs(repository + "/" + exp_name,currentIndIndex,wheels,joints,sensors);
    int nn_inputs = sensors*2;
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
    auto& ctrl_gen = ctrl_archive.archive[wheels][joints][sensors];
    CMAESLearner learner(nbr_weights,nbr_bias,nn_inputs,nn_outputs);
    if(ctrl_gen.first->get_weights().empty() && ctrl_gen.first->get_weights().empty())
        learner.init();
    else learner.init(ctrl_gen.first->get_full_genome());
    learners.emplace(id,learner);
}

const Genome::Ptr &MNIPES::get_next_controller_genome(int id){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#nbrHiddenNeurons").value;

    NN2Control<elman_t>::Ptr ctrl(new NN2Control<elman_t>);
    auto nn_params = learners[id].update_ctrl(ctrl);
    NNParamGenome::Ptr gen;
    gen->set_weights(nn_params.first);
    gen->set_biases(nn_params.second);
    gen->set_nbr_hidden(nb_hidden);
    gen->set_nn_type(nn_type);
    gen->set_nbr_input(learners[id].get_nbr_inputs());
    gen->set_nbr_output(learners[id].get_nbr_outputs());
    return gen;
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
    std::string repo = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    phy::write_morph_blueprints<PMEIndividual>(repo + "/" + exp_name,population);

    phy::write_morph_meshes<PMEIndividual>(repo + "/" + exp_name,population);
}

void MNIPES::load_data_for_update() {
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;

    //load ids to be evaluated
    std::vector<int> list_ids;
    phy::load_ids_to_be_evaluated(repository + "/" + exp_name,list_ids);

    //TODO: load saved learners

    //load controller archive
    int max_nbr_organs = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;
    ctrl_archive.init(max_nbr_organs,max_nbr_organs,max_nbr_organs);
    ctrl_archive.from_file(repository + "/" + exp_name + "/logs/controller_archive");

    //create a first genome controller for the first robot chosen
    init_learner(currentIndIndex);
}

void MNIPES::write_data_for_update(){

}
