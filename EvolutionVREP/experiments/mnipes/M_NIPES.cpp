#include "M_NIPES.hpp"

using namespace are;

void ControllerArchive::init(int max_wheels, int max_joints, int max_sensors)
{
    archive.resize(max_wheels+1);
    for(auto& a_w : archive){
        a_w.resize(max_joints+1);
        for(auto& a_j: a_w){
            a_j.resize(max_sensors+1);
        }
    }

    for(auto& w : archive){
        for(auto& j : w){
            for(auto& s : j){
                s.first.reset(new NNParamGenome());
                s.second = 0;
            }
        }
    }
}

void ControllerArchive::update(const NNParamGenome::Ptr &genome, double fitness, int wheels, int joints, int sensors){
    if(archive[wheels][joints][sensors].second < fitness)
        archive[wheels][joints][sensors] = std::make_pair(genome,fitness);
}

void M_NIPESIndividual::createMorphology(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();

    morphology.reset(new Morphology_CPPNMatrix(parameters));
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);
    float pos[3];
    simGetObjectPosition(morphology->getMainHandle(),-1,pos);
    setGenome();
    setMorphDesc();
    setManRes();
    setManScore();
    setGraphMatrix();
    setSymDesc();
}

void M_NIPESIndividual::createController(){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;

    //If first evaluation on this morphology : init the learner (CMAES)
    if(std::dynamic_pointer_cast<CMAESLearner>(learner)->get_nbr_eval() == 0)
    {
        int wheel_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getIndDesc().cartDesc.wheelNumber;
        int joint_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getIndDesc().cartDesc.jointNumber;
        int sensor_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->getIndDesc().cartDesc.sensorNumber;

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

        auto& starting_gen = controller_archive.archive[wheel_nbr][joint_nbr][sensor_nbr].first;

        if(starting_gen->get_weights().empty() && starting_gen->get_biases().empty())
            std::dynamic_pointer_cast<CMAESLearner>(learner)->init();
        else{
            std::vector<double> init_pt = std::dynamic_pointer_cast<NNParamGenome>(starting_gen)->get_full_genome();
            std::dynamic_pointer_cast<CMAESLearner>(learner)->init(init_pt);
        }
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
    energy_cost += morphology->get_energy_cost();
    if(std::isnan(energy_cost))
        energy_cost = 0;
    sim_time = delta_time;
}

void M_NIPESIndividual::setGenome()
{
    nn = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getGenome();
}

void M_NIPESIndividual::setMorphDesc()
{
    morphDesc = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getMorphDesc();
}

void M_NIPESIndividual::setManRes()
{
    testRes = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getRobotManRes();
}

void M_NIPESIndividual::setManScore()
{
    manScore = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getManScore();
}

void M_NIPESIndividual::setGraphMatrix()
{
    graphMatrix =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getGraphMatrix();
}

void M_NIPESIndividual::setSymDesc()
{
    symDesc =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getSymDesc();
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

    //Initialized the population of morphologies
    if(!simulator_side || instance_type == settings::INSTANCE_REGULAR){

        int max_wheels = settings::getParameter<settings::Integer>(parameters,"#maxNbrWheels").value;
        int max_joints = settings::getParameter<settings::Integer>(parameters,"#maxNbrJoints").value;
        int max_sensors = settings::getParameter<settings::Integer>(parameters,"#maxNbrSensors").value;
        controller_archive.init(max_wheels,max_joints,max_sensors);
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
    //Load the bootstrap population
    listMorphGenomeID(morphIDList);
    unsigned int pop_size = morphIDList.size();

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
    for(unsigned i = 0; i < pop_size; i++){
        loadNEATGenome(morphIDList[i],morph_gen);
        morph_population->AccessGenomeByIndex(i) = morph_gen;
    }
    for(int i = 0; i < pop_size ; i++){
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

    int max_wheel = settings::getParameter<settings::Integer>(parameters,"#maxNbrWheels").value;
    int max_joint = settings::getParameter<settings::Integer>(parameters,"#maxNbrJoints").value;
    int max_sensor = settings::getParameter<settings::Integer>(parameters,"#maxNbrSensors").value;
    //update controller archive
    for(const auto& ind: population){
        const Eigen::VectorXd &morph_desc = std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->getMorphDesc();
        auto ctrl_gen = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome());
        controller_archive.update(ctrl_gen,ind->getObjectives()[0],morph_desc[4]*max_wheel,morph_desc[6]*max_joint,morph_desc[5]*max_sensor);
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

bool M_NIPES::update(const Environment::Ptr& env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    if(simulator_side){
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->set_trajectory(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_trajectory());
        std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner())->update_pop_info(
                        ind->getObjectives(),
                        std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->descriptor()
                    );

        //LEARNING WITH NIP-ES
        if(!std::dynamic_pointer_cast<CMAESLearner>(ind->get_learner())->step()){
          //  std::dynamic_pointer_cast<M_NIPESIndividual>(ind)->update_ctrl();
            return false;
        }else return true;
    }else return true;
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
        md.wheels = static_cast<int>(std::stod(split_line[5])*10);
        md.joints = static_cast<int>(std::stod(split_line[7])*10);
        md.sensors = static_cast<int>(std::stod(split_line[6])*10);
        full_desc_map.emplace(id,md);
    }

    for(const int &id : list)
        desc_map.emplace(id,full_desc_map[id]);
}


bool M_NIPES::finish_eval(){
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

    int handle = population[currentIndIndex]->get_morphology()->getMainHandle();
    float pos[3];
    simGetObjectPosition(handle,-1,pos);
    double dist = distance(pos,tPos)/sqrt(2*arenaSize*arenaSize);

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
    bool stop = dist < fTarget || drop_eval;

    if(stop){
        std::cout << "STOP !" << std::endl;
    }

    return  stop;
}
