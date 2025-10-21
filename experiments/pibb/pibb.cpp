#include "pibb.hpp"
#include "are_torch/torch_nn.hpp"
#include "simulatedER/are_morphology.hpp"
#include "simulatedER/FixedMorphology.hpp"

using namespace are;

torch::Tensor TensorGenome::get_section(int i){
    if (i > _sections.size())
        throw std::runtime_error("Error TensorGenome::get_sections: out of bound");
    int l = (i - 1) > 0 ? _sections[i-1] : 0;
    return torch::slice(_tensor,0,l,_sections[i]+l);
}

std::string TensorGenome::to_string() const{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<TensorGenome>();
    oarch << *this;
    return sstream.str();
}

void TensorGenome::from_string(const std::string &str){
    std::stringstream sstream;
    sstream.str(str);
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<TensorGenome>();
    iarch >> *this;
}

void TensorGenome::from_file(const std::string &str){
    std::ifstream ifs(str);
    if(!ifs.is_open()){
        std::cerr << "Error TensorGenome::from_file: cannot open file " << str << std::endl;
        return;
    }
    boost::archive::text_iarchive iarch(ifs);
    iarch.register_type<TensorGenome>();
    iarch >> *this;
    ifs.close();
}


void PiBBIndividual::createController(){
    std::string fixed_morph_path = settings::getParameter<settings::String>(parameters,"#robotPath").value;

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    int nb_rbf = settings::getParameter<settings::Integer>(parameters,"#NbrRBFNeurones").value;
    const std::vector<int> joint_subs = settings::getParameter<settings::Sequence<int>>(parameters,"#jointSubs").value;

    int wheel_nbr,joint_nbr,sensor_nbr;
    if(fixed_morph_path == "None"){
        wheel_nbr = std::dynamic_pointer_cast<sim::AREMorphology>(morphology)->get_wheel_number();
        joint_nbr = std::dynamic_pointer_cast<sim::AREMorphology>(morphology)->get_joint_number();
        sensor_nbr = std::dynamic_pointer_cast<sim::AREMorphology>(morphology)->get_sensor_number();
    }else{
        wheel_nbr = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_wheelHandles().size();
        joint_nbr = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_jointHandles().size();
        sensor_nbr = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_proxHandles().size();
    }
    bool use_ir = settings::getParameter<settings::Boolean>(parameters,"#useIR").value;
    bool use_camera = settings::getParameter<settings::Boolean>(parameters,"#useCamera").value;
    bool use_joint_feedback = settings::getParameter<settings::Boolean>(parameters,"#useJointFeedback").value;
    bool use_wheel_feedback = settings::getParameter<settings::Boolean>(parameters,"#useWheelFeedback").value;

    int nb_inputs = sensor_nbr + //proximity sensors
                    (use_ir ? sensor_nbr : 0) + // IR sensors
                    (use_joint_feedback ? joint_nbr : 0) + // joint positions
                    (use_wheel_feedback ? wheel_nbr : 0) + // wheel positions
                    (use_camera ? 1 : 0); // camera
    int nb_outputs = wheel_nbr + joint_nbr;


    if(nn_type == tnn::t_MLP){
        std::cout << "Creating MLP with " << nb_inputs << " inputs, " << nb_hidden << " hidden neurons, and " << nb_outputs << " ouputs." << std::endl;
        torch::Tensor weights = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(0);
        torch::Tensor biases = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(1);
        control = std::make_shared<MLPControl>(nb_inputs,nb_outputs,nb_hidden);
        std::dynamic_pointer_cast<MLPControl>(control)->_nn->set_weights_biases(weights,biases);
    }else if(nn_type == tnn::t_RNN){
        std::cout << "Creating RNN with " << nb_inputs << " inputs, " << nb_hidden << " hidden neurons, and " << nb_outputs << " ouputs." << std::endl;
        torch::Tensor weights = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(0);
        torch::Tensor biases = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(1);
        control = std::make_shared<RNNControl>(nb_inputs,nb_outputs,nb_hidden);
        std::dynamic_pointer_cast<RNNControl>(control)->_nn->set_weights_biases(weights,biases);
    }else if(nn_type == tnn::t_CPGRBF){
        std::cout << "Creating CPGRBFNetwork with " << nb_rbf <<" RBF neurons, and " << nb_outputs << " ouputs." << std::endl;
        torch::Tensor weights = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(0);
        torch::Tensor biases = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(1);
        control = std::make_shared<CPGRBFControl>(nb_outputs,nb_rbf);
        std::dynamic_pointer_cast<CPGRBFControl>(control)->_nn->set_out_layer_parameters(weights,biases);
    }else if(nn_type == tnn::t_CPGRBFRNN){
        std::cout << "Creating CPGRBFRNN with " << nb_inputs << " inputs, " << nb_hidden << " recurrent neurons, " << nb_rbf <<" RBF neurons, and " << nb_outputs << " ouputs." << std::endl;
        torch::Tensor rnn_ws = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(0);
        torch::Tensor out_ws = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(1);
        torch::Tensor rnn_bs = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(2);
        torch::Tensor out_bs = std::dynamic_pointer_cast<TensorGenome>(ctrlGenome)->get_section(3);

        control = std::make_shared<CPGRBFRNNControl>(nb_inputs,nb_outputs,nb_rbf, nb_hidden); 
        std::dynamic_pointer_cast<CPGRBFRNNControl>(control)->_nn->set_out_layer_parameters(out_ws,out_bs);
        std::dynamic_pointer_cast<CPGRBFRNNControl>(control)->_nn->set_rnn_parameters(rnn_ws,rnn_bs);
        
    }
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }
    control->set_parameters(parameters);
    control->set_random_number(randNum);

}

void PiBBIndividual::createMorphology(){
    morphology = std::make_shared<sim::FixedMorphology>(parameters);
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    morphology->set_randNum(randNum);

    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;

    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);
    std::dynamic_pointer_cast<sim::Morphology>(morphology)->reset_actuators();
}

Eigen::VectorXd PiBBIndividual::descriptor()
{
    if(descriptor_type == FINAL_POSITION){
        std::vector<double> arena_size = settings::getParameter<settings::Sequence<double>>(parameters,"#arenaSize").value;
        Eigen::VectorXd desc(3);
        desc << (final_position[0]+arena_size[0]/2.)/arena_size[0], (final_position[1]+arena_size[1]/2.)/arena_size[1], final_position[2];
        return desc;
    }else if(descriptor_type == VISITED_ZONES){
        Eigen::MatrixXd vz = visited_zones.cast<double>();
        Eigen::VectorXd desc(Eigen::Map<Eigen::VectorXd>(vz.data(),vz.cols()*vz.rows()));
        return desc;
    }else throw std::invalid_argument("NIPESIndvidual::descriptor(): descriptor type unknown");

}

std::string PiBBIndividual::to_string() const
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<PiBBIndividual>();
    oarch.register_type<TensorGenome>();
    oarch << *this;
    return sstream.str();
}

void PiBBIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<PiBBIndividual>();
    iarch.register_type<TensorGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}

void PiBBIndividual::compute_fitness(){
    double fitness = 0;
    for(const auto &r : rewards)
        fitness += r;
    fitness /= static_cast<double>(rewards.size());
    objectives[0] = fitness;
//    copy_rewards = rewards;
}



void PiBB::init(){
    if(!simulator_side)
    {
        int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        double sigma = settings::getParameter<settings::Double>(parameters,"#sigma").value;

        int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
        const int nb_input = settings::getParameter<settings::Integer>(parameters,"#NbrInputNeurones").value;
        const int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
        const int nb_rbf = settings::getParameter<settings::Integer>(parameters,"#NbrRBFNeurones").value;
        const int nb_output = settings::getParameter<settings::Integer>(parameters,"#NbrOutputNeurones").value;
        const std::vector<int> joint_subs = settings::getParameter<settings::Sequence<int>>(parameters,"#jointSubs").value;

        std::vector<int> param_sizes(2);
        if(nn_type == tnn::t_MLP)
            MLPControl::nbr_parameters(nb_input,nb_output,nb_hidden,param_sizes[0],param_sizes[1]);
        else if(nn_type == tnn::t_RNN)
            RNNControl::nbr_parameters(nb_input,nb_output,nb_hidden,param_sizes[0],param_sizes[1]);
        else if(nn_type == tnn::t_CPGRBF)
            CPGRBFControl::nbr_parameters(nb_output,nb_rbf,param_sizes[0],param_sizes[1]);
        else if(nn_type == tnn::t_CPGRBFRNN){
            param_sizes.resize(4);
            CPGRBFRNNControl::nbr_parameters(nb_input,nb_output,nb_rbf,nb_hidden,param_sizes[0],param_sizes[1],param_sizes[2],param_sizes[3]);
        }
        else {
            std::cerr << "unknown type of neural network" << std::endl;
            return;
        }

        int sum = 0;
        for(int size: param_sizes)
            sum += size;
        _pibb = std::make_unique<l::PiBB>();
        _pibb->init(pop_size,sum,sigma,true);
        _pibb->set_rand_num(randomNum);
        _pibb->set_inverse_lambda(1);
        

        std::vector<double> initial_policy = randomNum->randVectd(-1,1,sum);
        _pibb->policy_params() = torch::from_blob(initial_policy.data(),{static_cast<int64_t>(initial_policy.size())},torch::TensorOptions().dtype(torch::kDouble)).clone();

        torch::Tensor samples;
        _pibb->generate_samples(samples);

        for(int i = 0; i < pop_size; i++){
            EmptyGenome::Ptr morph_gen = std::make_shared<EmptyGenome>();
            TensorGenome::Ptr ctrl_gen = std::make_shared<TensorGenome>();
            ctrl_gen->set_tensor(samples[i]);
            ctrl_gen->set_sections(param_sizes);
            Individual::Ptr ind = std::make_shared<PiBBIndividual>(morph_gen,ctrl_gen);
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }
        //Add initial policy
        EmptyGenome::Ptr morph_gen = std::make_shared<EmptyGenome>();
        TensorGenome::Ptr ctrl_gen = std::make_shared<TensorGenome>();
        ctrl_gen->set_tensor(_pibb->policy_params());
        ctrl_gen->set_sections(param_sizes);
        _current_policy_ind = std::make_shared<PiBBIndividual>(morph_gen,ctrl_gen);
        _current_policy_ind->set_parameters(parameters);
        _current_policy_ind->set_randNum(randomNum);
        _current_policy_ind->set_is_current_policy(true);
        population.push_back(_current_policy_ind);
    }else{
        EmptyGenome::Ptr morph_gen = std::make_shared<EmptyGenome>();
        TensorGenome::Ptr ctrl_gen = std::make_shared<TensorGenome>();
        PiBBIndividual::Ptr ind = std::make_shared<PiBBIndividual>(morph_gen,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
}

void PiBB::epoch(){
    std::cout << numberEvaluation << "/" << settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value << " evaluations" << std::endl;
    if(!_pibb->iterate()){
        std::cerr << "Error PiBB::epoch: iteration failed" << std::endl;
        exit(1);
    }
}

void PiBB::init_next_pop(){
    torch::Tensor samples;
    _pibb->generate_samples(samples);
    std::vector<int> param_sizes = std::dynamic_pointer_cast<TensorGenome>(population[0]->get_ctrl_genome())->get_sections();

    population.clear();
    //Add exploration samples
    for(int i = 0; i < _pibb->get_sample_size() ; i++){
        EmptyGenome::Ptr morph_gen = std::make_shared<EmptyGenome>();
        TensorGenome::Ptr ctrl_gen = std::make_shared<TensorGenome>();
        ctrl_gen->set_tensor(samples[i]);
        ctrl_gen->set_sections(param_sizes);
        Individual::Ptr ind = std::make_shared<PiBBIndividual>(morph_gen,ctrl_gen);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }
    //Add current policy
    EmptyGenome::Ptr morph_gen = std::make_shared<EmptyGenome>();
    TensorGenome::Ptr ctrl_gen = std::make_shared<TensorGenome>();
    ctrl_gen->set_tensor(_pibb->policy_params());
    ctrl_gen->set_sections(param_sizes);
    _current_policy_ind.reset();
    _current_policy_ind = std::make_shared<PiBBIndividual>(morph_gen,ctrl_gen);
    _current_policy_ind->set_parameters(parameters);
    _current_policy_ind->set_randNum(randomNum);
    _current_policy_ind->set_is_current_policy(true);
    population.push_back(_current_policy_ind);
}

void PiBB::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    int env_type = settings::getParameter<settings::Integer>(parameters,"#envType").value;
    if((env_type == sim::MULTI_TARGETS || env_type == sim::BARREL) && simulator_side){//MultiTargetMaze
        std::dynamic_pointer_cast<PiBBIndividual>(population[indIdx])->add_reward(objectives[0]);
    }
    population[indIdx]->setObjectives(objectives);
    newly_evaluated.push_back(indIdx);
}


bool PiBB::update(const Environment::Ptr & env){
    if(!simulator_side){
        for(const int& idx :newly_evaluated)
        {
            if(population[idx]->getObjectives()[0] > 0 && !std::dynamic_pointer_cast<PiBBIndividual>(population[idx])->is_current_policy()){
                numberEvaluation++;
                reevaluated++;
                _pibb->step(population[idx]->get_rollout(),
                            std::dynamic_pointer_cast<PiBBIndividual>(population[idx])->get_immediate_returns(),
                            population[idx]->getObjectives()[0]);
            }
            
        }
        newly_evaluated.clear();
    }

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_final_position(env->get_final_position());
        std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_trajectory(env->get_trajectory());
        std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_immediate_returns(
            std::dynamic_pointer_cast<sim::VirtualEnvironment>(env)->get_immediate_return());
        if(env->get_name() == "obstacle_avoidance" || env->get_name() == "exploration"){
            std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_visited_zones(std::dynamic_pointer_cast<sim::ObstacleAvoidance>(env)->get_visited_zone_matrix());
            std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_descriptor_type(VISITED_ZONES);
        }else if(/*env->get_name() == "multi_target_maze" ||*/ env->get_name() == "barrel_task"){
            int number_of_targets = 0;
            // if(env->get_name() == "multi_target_maze")
            //     number_of_targets = std::dynamic_pointer_cast<sim::MultiTargetMaze>(env)->get_number_of_targets();
            if(env->get_name() == "barrel_task")
                number_of_targets = std::dynamic_pointer_cast<sim::BarrelTask>(env)->get_number_of_targets();
            if(std::dynamic_pointer_cast<PiBBIndividual>(ind)->get_number_times_evaluated() < number_of_targets){
                return false;
            }else{
                std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_final_position(env->get_final_position());
                std::dynamic_pointer_cast<PiBBIndividual>(ind)->compute_fitness();
                //std::dynamic_pointer_cast<NIPESIndividual>(ind)->reset_rewards();
    //            std::dynamic_pointer_cast<sim::NN2Individual>(ind)->set_trajectories(std::dynamic_pointer_cast<sim::MultiTargetMaze>(env)->get_trajectories());
                // if(env->get_name() == "multi_target_maze")
                    // std::dynamic_pointer_cast<NIPESIndividual>(ind)->set_trajectories(std::dynamic_pointer_cast<sim::MultiTargetMaze>(env)->get_trajectories());
                if(env->get_name() == "barrel_task")
                    std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_trajectories(std::dynamic_pointer_cast<sim::BarrelTask>(env)->get_trajectories());            }
        }else if(env->get_name() == "push_object")
            std::dynamic_pointer_cast<PiBBIndividual>(ind)->set_object_trajectory(std::dynamic_pointer_cast<sim::PushObject>(env)->get_object_trajectory());
    }

//    int nbReEval = settings::getParameter<settings::Integer>(parameters,"#numberOfReEvaluation").value;
//    if(reevaluated < nbReEval)
//        return false;

//    reevaluated = 0;
    return true;
}

bool PiBB::is_finish(){
    int maxNbrEval = settings::getParameter<settings::Integer>(parameters,"#maxNbrEval").value;
    return /*_is_finish ||*/ numberEvaluation >= maxNbrEval;
}

bool PiBB::finish_eval(const Environment::Ptr & env){
//    std::vector<double> target = settings::getParameter<settings::Sequence<double>>(parameters,"#targetPosition").value;
//    double t_pos[3] = {target[0],target[1],target[2]};
//    double fTarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
//    double arenaSize = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;

//    auto distance = [](double* a,double* b) -> double
//    {
//        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
//                         (a[1] - b[1])*(a[1] - b[1]) +
//                         (a[2] - b[2])*(a[2] - b[2]));
//    };

//    int handle = std::dynamic_pointer_cast<sim::Morphology>(population[currentIndIndex]->get_morphology())->getMainHandle();
//    float pos[3];
//    simGetObjectPosition(handle,-1,pos);
//    double posd[3];
//    posd[0] = static_cast<double>(pos[0]);
//    posd[1] = static_cast<double>(pos[1]);
//    posd[2] = static_cast<double>(pos[2]);

//    double dist = distance(posd,t_pos)/sqrt(2*arenaSize*arenaSize);

//    if(dist < fTarget){
//        std::cout << "STOP !" << std::endl;
//    }

//    return  dist < fTarget;
    return false;
}


