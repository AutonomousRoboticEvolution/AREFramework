#include "PMNIPES.hpp"

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

void PMNIPESIndividual::createMorphology(){
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();

    morphology.reset(new Morphology_CPPNMatrix(parameters));
    morphology->set_randNum(randNum);
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    float init_z = settings::getParameter<settings::Float>(parameters,"#init_z").value;

    morphology->createAtPosition(init_x,init_y,init_z);
    setGenome();
    setMorphDesc();
    setManRes();
}

void PMNIPESIndividual::createController(){
    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;

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

void PMNIPESIndividual::update(double delta_time){
    std::vector<double> inputs = morphology->update();
    std::vector<double> outputs = control->update(inputs);

    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->command(outputs);
//    energy_cost += morphology->get_energy_cost();
//    if(std::isnan(energy_cost))
//        energy_cost = 0;
    sim_time = delta_time;
}

void PMNIPESIndividual::setGenome()
{
    nn = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getGenome();
}

void PMNIPESIndividual::setMorphDesc()
{
    morphDesc = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getMorphDesc();
}

void PMNIPESIndividual::setManRes()
{
    testRes = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getRobotManRes();
}

std::string PMNIPESIndividual::to_string()
{
    
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<PMNIPESIndividual>();
    oarch.register_type<CPPNGenome>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    std::cout << "Sent" << std::endl;
    std::cout << sstream.str() << std::endl;
    return sstream.str();
}

void PMNIPESIndividual::from_string(const std::string &str){
    std::cout << "Recieved" << std::endl; 
    std::cout << str << std::endl;
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<PMNIPESIndividual>();
    iarch.register_type<CPPNGenome>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;

    //set parameters and randNum to the genome as it is not contained in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}


void PMNIPES::init(){


    Novelty::k_value = settings::getParameter<settings::Integer>(parameters,"#kValue").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;
    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProb").value;


    int max_nbr_organs = settings::getParameter<settings::Integer>(parameters,"#maxNbrOrgans").value;

    bool start_from_exp = settings::getParameter<settings::Boolean>(parameters,"#loadPrevExperiment").value;
    if(start_from_exp){
        std::string start_from_exp_folder = settings::getParameter<settings::String>(parameters,"#startFromExperiment").value;
        std::vector<short> pastMorphIDlist;
        std::vector<std::string> str_split;
        for(const auto& dir : boost::make_iterator_range(boost::filesystem::directory_iterator(start_from_exp_folder),{})){
            boost::split(str_split,dir.path().filename().string(),boost::is_any_of("_"));
            if(str_split[0] == "morph"){
                pastMorphIDlist.push_back(std::stoi(str_split[1]));
            }
        }

        listMorphGenomeID(morphIDList,pastMorphIDlist);
    }else listMorphGenomeID(morphIDList);


    controller_archive.init(max_nbr_organs,max_nbr_organs,max_nbr_organs);

    loadNbrSenAct(morphIDList,morphDescMap);

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    const int nb_input =  morphDescMap[morphIDList[0]].sensors*2;
    const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const int nb_output = morphDescMap[morphIDList[0]].wheels + morphDescMap[morphIDList[0]].joints;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

    init_cmaes(nbr_weights+nbr_bias);

    init_first_pop(nbr_weights,nbr_bias);

    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    if(!simulator_side || instance_type == settings::INSTANCE_REGULAR){
        std::stringstream sstr;
        sstr << "morph_" << morphIDList[morphCounter];
        sub_folder = sstr.str();
        if(!boost::filesystem::exists(Logging::log_folder + std::string("/") + sub_folder))
            boost::filesystem::create_directory(Logging::log_folder + std::string("/") + sub_folder);
    }

}

void PMNIPES::init_cmaes(int dim){
    int lenStag = settings::getParameter<settings::Integer>(parameters,"#lengthOfStagnation").value;

    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;
    double step_size = settings::getParameter<settings::Double>(parameters,"#CMAESStep").value;
    double ftarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool elitist_restart = settings::getParameter<settings::Boolean>(parameters,"#elitistRestart").value;
    double novelty_ratio = settings::getParameter<settings::Double>(parameters,"#noveltyRatio").value;
    double novelty_decr = settings::getParameter<settings::Double>(parameters,"#noveltyDecrement").value;
    float pop_stag_thres = settings::getParameter<settings::Float>(parameters,"#populationStagnationThreshold").value;

    std::vector<double> initial_point = randomNum->randVectd(-max_weight,max_weight,dim);

    double lb[dim], ub[dim];
    for(int i = 0; i < dim; i++){
        lb[i] = -max_weight;
        ub[i] = max_weight;
    }

    geno_pheno_t gp(lb,ub,dim);

    cma::CMAParameters<geno_pheno_t> cmaParam(initial_point,step_size,pop_size,randomNum->getSeed(),gp);
    cmaParam.set_ftarget(ftarget);
    cmaParam.set_quiet(!verbose);

    cmaStrategy.reset(new IPOPCMAStrategy([](const double*,const int&)->double{},cmaParam));
    cmaStrategy->set_elitist_restart(elitist_restart);
    cmaStrategy->set_length_of_stagnation(lenStag);
    cmaStrategy->set_novelty_ratio(novelty_ratio);
    cmaStrategy->set_novelty_decr(novelty_decr);
    cmaStrategy->set_pop_stag_thres(pop_stag_thres);

}

void PMNIPES::init_first_pop(int nbr_weights, int nbr_bias){

    int pop_size = cmaStrategy->get_parameters().lambda();

    dMat init_samples = cmaStrategy->ask();

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);

    loadNEATGenome(morphIDList[morphCounter],current_morph_gen);

    for(int u = 0; u < pop_size; u++){
        for(int v = 0; v < nbr_weights; v++)
            weights[v] = init_samples(v,u);
        for(int w = nbr_weights; w < nbr_weights+nbr_bias; w++)
            biases[w-nbr_weights] = init_samples(w,u);

        CPPNGenome::Ptr morph_gen(new CPPNGenome(current_morph_gen));
        morph_gen->set_parameters(parameters);
        morph_gen->set_randNum(randomNum);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        ctrl_gen->set_weights(weights);
        ctrl_gen->set_biases(biases);
        Individual::Ptr ind(new PMNIPESIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::dynamic_pointer_cast<PMNIPESIndividual>(ind)->set_nn_inputs(morphDescMap[morphIDList[morphCounter]].sensors*2);
        std::dynamic_pointer_cast<PMNIPESIndividual>(ind)->set_nn_outputs(morphDescMap[morphIDList[morphCounter]].wheels + morphDescMap[morphIDList[morphCounter]].joints);
        population.push_back(ind);

    }
}

bool PMNIPES::update(const Environment::Ptr & env){

    NIPES::update(env);

    NNParamGenome::Ptr gen = std::dynamic_pointer_cast<NNParamGenome>(population[currentIndIndex]->get_ctrl_genome());
    double fit = population[currentIndIndex]->getObjectives()[0];

    controller_archive.update(gen,fit,morphDescMap[morphIDList[morphCounter]].wheels,
            morphDescMap[morphIDList[morphCounter]].joints,
            morphDescMap[morphIDList[morphCounter]].sensors);

    return true;
}

void PMNIPES::init_next_pop(){
    int maxNbrEval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;

    if(numberEvaluation >= maxNbrEval || _is_finish || nbr_dropped_eval >= 50)
    {
        population.clear();
        cmaStrategy->reset_search_state();
        morphCounter++;
        numberEvaluation=0;
        _is_finish=false;
    	generation=0;
        std::stringstream sstr;
        sstr << "morph_" << morphIDList[morphCounter];
        sub_folder = sstr.str();
        if(!boost::filesystem::exists(Logging::log_folder + std::string("/") + sub_folder))
            boost::filesystem::create_directory(Logging::log_folder + std::string("/") + sub_folder);

        int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
        const int nb_input =  morphDescMap[morphIDList[morphCounter]].sensors*2;
        const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
        const int nb_output = morphDescMap[morphIDList[morphCounter]].wheels + morphDescMap[morphIDList[morphCounter]].joints;

        int nbr_weights, nbr_bias;
        if(nn_type == settings::nnType::FFNN)
            NN2Control<ffnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
        else if(nn_type == settings::nnType::RNN)
            NN2Control<rnn_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
        else if(nn_type == settings::nnType::ELMAN)
            NN2Control<elman_t>::nbr_parameters(nb_input,nb_hidden,nb_output,nbr_weights,nbr_bias);
        else {
            std::cerr << "unknown type of neural network" << std::endl;
            return;
        }

        init_cmaes(nbr_weights+nbr_bias);

        init_first_pop(nbr_weights,nbr_bias);

        return;
    }



    int pop_size = cmaStrategy->get_parameters().lambda();

    dMat new_samples = cmaStrategy->ask();

    int nbr_weights = std::dynamic_pointer_cast<NNParamGenome>(population[0]->get_ctrl_genome())->get_weights().size();
    int nbr_bias = std::dynamic_pointer_cast<NNParamGenome>(population[0]->get_ctrl_genome())->get_biases().size();

    std::vector<double> weights(nbr_weights);
    std::vector<double> biases(nbr_bias);

    population.clear();
    for(int i = 0; i < pop_size ; i++){

        for(int j = 0; j < nbr_weights; j++)
            weights[j] = new_samples(j,i);
        for(int j = nbr_weights; j < nbr_weights+nbr_bias; j++)
            biases[j-nbr_weights] = new_samples(j,i);

        CPPNGenome::Ptr morph_gen(new CPPNGenome(current_morph_gen));
        morph_gen->set_parameters(parameters);
        morph_gen->set_randNum(randomNum);
        NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
        ctrl_gen->set_weights(weights);
        ctrl_gen->set_biases(biases);
        Individual::Ptr ind(new PMNIPESIndividual(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        std::dynamic_pointer_cast<PMNIPESIndividual>(ind)->set_nn_inputs(morphDescMap[morphIDList[morphCounter]].sensors*2);
        std::dynamic_pointer_cast<PMNIPESIndividual>(ind)->set_nn_outputs(morphDescMap[morphIDList[morphCounter]].wheels +morphDescMap[morphIDList[morphCounter]].joints);
        population.push_back(ind);
    }
}

bool PMNIPES::is_finish(){
    int maxNbrEval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;

    return (numberEvaluation >= maxNbrEval || _is_finish || nbr_dropped_eval >= 50) && morphCounter >= morphIDList.size();
}

void PMNIPES::loadNEATGenome(short int genomeID, NEAT::Genome &gen){
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::cout << "Loading genome: " << genomeID << "!" << std::endl;
    std::stringstream filepath;
    filepath << loadExperiment << "/morphGenome" << genomeID;
    gen = NEAT::Genome(filepath.str().c_str());
}

void PMNIPES::listMorphGenomeID(std::vector<short int>& list,const std::vector<short>& past_list){
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
        bool add_val=true;
        while(ss >> val) {
            // Add the current integer to the 'colIdx' column's values vector
            if(!past_list.empty())
                for(const short& id : past_list)
                    add_val = add_val && id != val;
            if(add_val)
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

void PMNIPES::loadNbrSenAct(std::vector<short> &list, std::map<short, morph_desc_t> &desc_map){

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

bool PMNIPES::finish_eval(){
    float time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value;

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
    float sim_time = simGetSimulationTime();
    if(sim_time < time_step){
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
    bool drop_eval = sim_time > 10.0 && move_counter <= 10;
    if(drop_eval) nbr_dropped_eval++;
    bool stop = dist < fTarget || drop_eval;

    if(stop){
        std::cout << "STOP !" << std::endl;
    }

    return  stop;
}

