#include "locomotion_ppo.hpp"

using namespace are;

void PPOInd::createMorphology(){
    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;

    morphology.reset(new sim::FixedMorphology(parameters));
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    morphology->set_randNum(randNum);

    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);;

    //    float pos[3];
    //    simGetObjectPosition(std::dynamic_pointer_cast<CPPNMorph>(morphology)->getMainHandle(),-1,pos);
}


void PPOInd::createController(){
    if(policy.is_empty()){
        nb_wheels = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_wheelHandles().size();
        nb_joints = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_jointHandles().size();
        nb_sensors = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_proxHandles().size();

        int nb_inputs = nb_sensors + nb_joints + nb_wheels;
        int nb_outputs = nb_joints + nb_wheels;

        policy = Policy(nb_inputs,nb_outputs,args.ortho_init,args.init_sampling_log_std);
        optimizer = std::make_unique<torch::optim::Adam>(policy->parameters(), torch::optim::AdamOptions(args.lr));

    }
    policy->train(false);
    rollout_buffer.Reset();
}

std::string PPOInd::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<PPOInd>();
    oarch.register_type<EmptyGenome>();
    oarch << *this;
    return sstream.str();
}

void PPOInd::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<PPOInd>();
    iarch.register_type<EmptyGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}

void PPOInd::update(double delta_time){
    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;
    double diff = delta_time/ctrl_freq - std::trunc(delta_time/ctrl_freq);
//    double input_noise_lvl = settings::getParameter<settings::Double>(parameters,"#inputNoiseLevel").value;
//    double output_noise_lvl = settings::getParameter<settings::Double>(parameters,"#outputNoiseLevel").value;
    if( diff < 0.1){
        std::vector<double> sensors = morphology->update();
        std::vector<double> joints = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_joints_positions();

        for(double &j: joints)
            j = 2.*j/M_PI;

        std::vector<double> wheels = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_wheels_positions();
        std::vector<double> inputs;
        inputs.insert(inputs.begin(),sensors.begin(),sensors.end());
        inputs.insert(inputs.end(),joints.begin(),joints.end());
        inputs.insert(inputs.end(),wheels.begin(),wheels.end());
//        for(double& v: inputs)
//            v = randNum->normalDist(v,input_noise_lvl);

        torch::Tensor obs = torch::tensor(inputs);
                std::cout << "inputs: ";
                for(const double& i : inputs)
                    std::cout << i << ";";
                std::cout << std::endl;



        // Use policy to predict an action
        torch::Tensor action, value, log_prob;
        {
            torch::NoGradGuard no_grad;
            std::tie(action, value, log_prob) = policy(obs,true);
        }


        torch::Tensor tmp_act = action.contiguous();
        tmp_act = tmp_act.cpu();
        std::vector<double> outputs(tmp_act.data_ptr<float>(), tmp_act.data_ptr<float>() + tmp_act.numel());

//        for(double &o: outputs)
//            o = randNum->normalDist(o,output_noise_lvl);

        std::cout << "outputs: ";
        for(const double& o : outputs)
            std::cout << o << ";";
        std::cout << std::endl;
        morphology->command(outputs);

        torch::Tensor reward = torch::tensor(sim::Locomotion::instant_reward(std::make_shared<PPOInd>(*this)));

        float max_eval_time = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
        TerminalState ts = TerminalState::NotTerminal;
        if(delta_time>=max_eval_time)
            ts = TerminalState::Terminal;
        rollout_buffer.Add(obs,action,value,log_prob,reward,{ts});
    }
}



void RL_PPO::init(){
    try
    {
        assert(torch::cuda::is_available());
        args.seed = randomNum->getSeed();
        args.n_envs = 1;
        args.normalize_env_obs = true;
        args.normalize_env_reward = true;

        args.batch_size = 256;
        args.n_steps = 8;
        args.gamma = 0.9999f;
        args.lr = 1e-4f;
        args.entropy_loss_weight = 5e-3f;
        args.clip_value = 0.1f;
        args.n_epochs = 10;
        args.lambda_gae = 0.9f;
        args.max_grad_norm = 5.0f;
        args.val_loss_weight = 0.2f;
        args.init_sampling_log_std = -3.0f;
        args.ortho_init = true;

        torch::manual_seed(randomNum->getSeed());

        Genome::Ptr morph_gen = std::make_shared<EmptyGenome>();
        Genome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();

        Individual::Ptr ind = std::make_shared<PPOInd>(morph_gen,ctrl_gen,args);
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);

        morph_gen.reset();
        ctrl_gen.reset();

        logger = std::make_unique<Logger>((args.exp_path + "training_logs.csv"), true, false); //TODO logger

        env.SetTraining(true);
        env.Reset();

        start_time = std::chrono::steady_clock::now();

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }


}

bool RL_PPO::update(const Environment::Ptr &){

    Individual::Ptr &ind = population[currentIndIndex];
    RolloutBuffer &rollout_buffer = std::dynamic_pointer_cast<PPOInd>(ind)->get_rollouts();
    Policy& policy = std::dynamic_pointer_cast<PPOInd>(ind)->get_policy();
    std::unique_ptr<torch::optim::Adam> &optimizer = std::dynamic_pointer_cast<PPOInd>(ind)->get_optimizer();
    torch::Tensor future_value;
    {
        torch::NoGradGuard no_grad;
        //get last observation
        auto obs = rollout_buffer.get(rollout_buffer.size().value()-1).observation;
        future_value = policy->PredictValues(obs);
    }
    rollout_buffer.ComputeReturnsAndAdvantage(future_value,args.gamma,args.lambda_gae);

    //auto [total_reward, total_steps, total_episodes] = _collect_rollouts(rollout_buffer);

    //has_average = total_episodes > 0;

    auto dataset = rollout_buffer.map(RolloutSampleBatchTransform());
    timestep += dataset.size().value();

    auto dataloader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(std::move(dataset), torch::data::DataLoaderOptions().batch_size(args.batch_size));

    float policy_loss_val = 0.0f;
    float value_loss_val = 0.0f;
    float entropy_loss_val = 0.0f;
    int num_batches = 0;

    policy->train(true);
    for (uint64_t i = 0; i < args.n_epochs; ++i)
    {
        for (auto& rollout_data : *dataloader)
        {
            auto [values, log_probs, entropy] = policy->EvaluateActions(rollout_data.observation, rollout_data.action);

            // Normalize advantages
            torch::Tensor advantages = rollout_data.advantage;
            advantages = (advantages - advantages.mean()) / (advantages.std() + 1e-8);

//            // Compute pi ratio (should be == 1 for the first iteration)
//            torch::Tensor ratio = torch::exp(log_probs - rollout_data.log_prob);

            // Clipped surrogate loss
//            torch::Tensor surrogate_loss_1 = advantages * ratio;
//            torch::Tensor surrogate_loss_2 = advantages * torch::clamp(ratio, 1.0f - args.clip_value, 1.0f + args.clip_value);
            torch::Tensor policy_loss = -advantages.mean();
            policy_loss_val += policy_loss.item<float>();

            // Value loss with TD(lambda)
            torch::Tensor value_loss = torch::mse_loss(rollout_data.returns, values);
            value_loss_val += value_loss.item<float>();

            // Entropy loss
            torch::Tensor entropy_loss = -torch::mean(entropy);
            entropy_loss_val += entropy_loss.item<float>();

            torch::Tensor loss = policy_loss + args.entropy_loss_weight * entropy_loss + args.val_loss_weight * value_loss;

            optimizer->zero_grad();
            loss.backward();
            if (args.max_grad_norm > 0.0f)
            {
                torch::nn::utils::clip_grad_norm_(policy->parameters(), args.max_grad_norm);
            }
            optimizer->step();
            num_batches += 1;
        }
    }
    iteration += num_batches;
    const float train_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() / 1000.0f;

    logger->Log(timestep, iteration, train_time,
        {
            { "Loss policy", policy_loss_val / num_batches },
            { "Loss value", value_loss_val / num_batches },
            { "Loss entropy", entropy_loss_val / num_batches }
        }
    );
//    if (has_average)
//    {
//        logger->Log(timestep, iteration, train_time,
//            {
//                {"Reward episode", total_reward / total_episodes },
//                {"Reward step", total_reward / total_steps },
//                {"Steps per episode", static_cast<float>(total_steps) / total_episodes }
//            }
//        );
//    }
}

bool RL_PPO::is_finish(){
    int total_timesteps = settings::getParameter<settings::Integer>(parameters,"#totalTimesteps").value;
    return timestep >= total_timesteps;
}


//std::tuple<float, uint64_t, uint64_t> RL_PPO::_collect_rollouts(RolloutBuffer &buffer){
//    policy->train(false);
//    buffer.Reset();

//    uint64_t t = 0;
//    float total_reward = 0.0f;
//    uint64_t total_steps = 0;
//    uint64_t total_end_episodes = 0;
//    VectorizedStepResult step_result;

//    // Get current observation
//    torch::Tensor obs = env.GetObs();

//    while (t < args.n_steps)
//    {
//        // Use policy to predict an action
//        torch::Tensor action, value, log_prob;
//        {
//            torch::NoGradGuard no_grad;
//            std::tie(action, value, log_prob) = policy(obs);
//        }

//        // Perform action in the env
//        step_result = env.Step(action);

//        bool has_env_timeout = false;
//        for (uint64_t i = 0; i < step_result.terminal_states.size(); ++i)
//        {
//            has_env_timeout = has_env_timeout || step_result.terminal_states[i] == TerminalState::Timeout;
//        }
//        // In case at least one of the envs timeout, approx potential future reward
//        // using policy estimation and add it to the reward for these envs
//        if (has_env_timeout)
//        {
//            torch::NoGradGuard no_grad;
//            torch::Tensor terminal_value = policy->PredictValues(step_result.obs);
//            for (uint64_t i = 0; i < step_result.terminal_states.size(); ++i)
//            {
//                if (step_result.terminal_states[i] == TerminalState::Timeout)
//                {
//                    step_result.rewards[i] += args.gamma * terminal_value[i].item<float>();
//                }
//            }
//        }

//        buffer.Add(obs, action, value, log_prob, step_result.rewards, step_result.terminal_states);

//        // Set the observation for the next step
//        obs = step_result.obs;
//        for (uint64_t i = 0; i < step_result.terminal_states.size(); ++i)
//        {
//            if (step_result.terminal_states[i] != TerminalState::NotTerminal)
//            {
//                obs[i] = step_result.new_episode_obs[i];
//                total_reward += step_result.episodes_tot_reward[i];
//                total_steps += step_result.episodes_tot_length[i];
//                total_end_episodes += 1;
//            }
//        }
//        t += 1;
//    }

//    // As the last episode might not be complete,
//    // approx potential future rewards using
//    // policy estimation
//    torch::Tensor future_value;
//    {
//        torch::NoGradGuard no_grad;
//        future_value = policy->PredictValues(obs);
//        for (uint64_t i = 0; i < future_value.size(0); ++i)
//        {
//            if (step_result.terminal_states[i] != TerminalState::NotTerminal)
//            {
//                future_value[i] = 0.0f;
//            }
//        }
//    }

//    buffer.ComputeReturnsAndAdvantage(future_value, args.gamma, args.lambda_gae);
//    return { total_reward, total_steps, total_end_episodes };
//}
