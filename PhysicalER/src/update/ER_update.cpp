#include "physicalER/update/ER_update.hpp"

using namespace are::phy::update;

void ER::initialize(){
    verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int seed = settings::getParameter<settings::Integer>(parameters,"#seed").value;

    if(seed < 0){
        std::random_device rd;
        seed = rd();
        are::settings::random::parameters->emplace("#seed",new are::settings::Integer(seed));
    }
    randNum = std::make_shared<are::misc::RandNum>(seed);

    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;

    Logging::create_folder(repository + std::string("/") + exp_name);
    Logging::create_folder(repository + std::string("/") + exp_name + std::string("/waiting_to_be_built/"));
    Logging::set_log_folder(repository + std::string("/") + exp_name);

    if (verbose) {
        std::cout << "ER initialize" << std::endl;
    }

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;

    std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(exp_plugin_name);

    if(!load_fct_exp_plugin<Environment::Factory>
            (environmentFactory,libhandler,"environmentFactory"))
        exit(1);
    environment = environmentFactory(parameters);
    environment->set_randNum(randNum);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,libhandler,"EAFactory"))
        exit(1);
    ea = EAFactory(randNum, parameters);
    ea->init();

    if(!load_fct_exp_plugin<Logging::Factory>
            (loggingFactory,libhandler,"loggingFactory"))
        exit(1);
    loggingFactory(logs,parameters);

    libhandler->close();

    if (verbose) std::cout << "ER initialized" << std::endl;
}

void ER::choice_of_robot(){
    //Load list of robot's ids to be evaluated and ask user which one want to be evaluated.
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    bool auto_mode = settings::getParameter<settings::Boolean>(parameters,"#autoMode").value;
    ioh::load_ids_to_be_evaluated(repository + "/" + exp_name,list_ids);
    if(list_ids.empty()){
        std::cout << "No robot available for evaluation." << std::endl;
        exit(1);
    }
    if(list_ids[0] != current_id){
        int i = 0;
        for(;i < list_ids.size(); i++)
            if(current_id == list_ids[i])
                break;
        if(i < list_ids.size()){
            list_ids[i] = list_ids[0];
            list_ids[0] = current_id;
        }
    }
    if(auto_mode)
        current_id = list_ids[0];
    else
        current_id = ioh::choice_of_robot_to_evaluate(list_ids);
    ea->setCurrentIndIndex(current_id);
    //-
}

void ER::load_data(){
    ea->load_data_for_update();
}

void ER::write_data(){
    ea->write_data_for_update();
}

void ER::save_logs(bool eog)
{
    for(const auto &log : logs)
        log->saveLog(ea);
}
bool ER::execute(){

    if(robot_state == READY){
        start_evaluation();
        robot_state = BUSY;
    }

    else if(robot_state == BUSY){
//        std::cout << "update eval" << std::endl;
        if(update_evaluation())
            robot_state = FINISHED;
    }

    else if(robot_state == FINISHED){
        robot_state = READY;
        return stop_evaluation();
    }
    else{
        std::cerr << "Unknown state" << std::endl;
    }
    return true;
}

void ER::start_evaluation(){
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    bool sim_mode = settings::getParameter<settings::Boolean>(parameters,"#simMode").value;

    if(verbose) std::cout << "Starting Evaluation for robot with ID: "<<current_id<<"\n=====" << std::endl;

    eval_t1 = std::chrono::steady_clock::now();

    if(!isEnvInit && !sim_mode){
        isEnvInit=true;
        environment->init();
    }

    // get the pi's IP address and the list of organs from the list_of_organs file
    std::string pi_address, list_of_organs;
    ioh::load_list_of_organs(repository + "/" + exp_name,current_id,pi_address,list_of_organs);

    if(sim_mode) pi_address = "localhost";

    //start ZMQ
    std::stringstream sstream1,sstream2;
    sstream1 << "tcp://" << pi_address << ":5556";
    sstream2 << "tcp://" << pi_address << ":5555";
    request.connect(sstream1.str().c_str());
    subscriber.connect(sstream2.str().c_str());
    subscriber.set(zmq::sockopt::subscribe, "pi ");

    //send the parameters as a string
    std::string reply;
    std::string param = settings::toString(*parameters.get());
    //std::cout<<"params : \n"<<param<<std::endl;
    send_string(reply,param,request,"pi ");
    assert(reply == "parameters_received");

    //send the list of organ addresses
    send_string(reply,list_of_organs,request,"pi ");
    assert(reply == "organ_addresses_received");

    std::string ctrl_gen = ea->get_next_controller_genome(current_id)->to_string();
    std::stringstream sstr;
    sstr << current_id << std::endl << ctrl_gen;
    //std::cout << ctrl_gen << std::endl;
    send_string(reply,sstr.str(),request,"pi ");
    assert(reply == "starting");
}

bool ER::update_evaluation(){
    //float eval_time = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;

    using namespace std::chrono;

    eval_t2 = steady_clock::now();
    duration<float> eval_duration = duration_cast<duration<float>>(eval_t2 - eval_t1);
    double current_time = duration_cast<seconds>(eval_duration).count();

    //currentInd->update(eval_duration.count());
    environment->update_info(current_time);

    std::string message_string;
    receive_string_no_reply(message_string,subscriber,"pi ");
    return message_string=="finish";

}

bool ER::stop_evaluation(){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool sim_mode = settings::getParameter<settings::Boolean>(parameters,"#simMode").value;

    if(verbose) std::cout << "individual " << current_id << " has finished evaluating" << std::endl;

    // get any logs that the robot has gathered:
    bool getting_logs=true;
    while(getting_logs){
        // get message:
        std::string message;
        receive_string_no_reply(message,subscriber,"pi ");

        // if message is "finshed_logs", then we continue
        if (message=="finished_logs"){
            getting_logs=false;
        }else{ // otherwise, this is a log packet
            //std::cout << "got a log" << message << std::endl;
            Logging::saveStringToFile( "log_file.txt" , message);
        }
    }
    std::vector<double> objs;
    std::vector<waypoint> traj;
    if(sim_mode){
        //receive the fitness
        std::string message;
        receive_string_no_reply(message,subscriber,"pi ");
//        std::cout << "Simulation mode, fitness obtained: " << message << std::endl;
        std::vector<std::string> split;
        misc::split_line(message,";",split);
        objs.resize(split.size());
        for(int i = 0; i < split.size(); i++)
            objs[i] = std::stod(split[i]);

        //receive the trajectory
        receive_string_no_reply(message,subscriber,"pi ");
//        std::cout << "Simulation mode, fitness obtained: " << message << std::endl;
        misc::split_line(message,"\n",split);
        traj.resize(split.size());
        for(int i = 0; i < split.size(); i++)
            traj[i].from_string(split[i]); ;
    }

    // display the fitness:
    environment->print_info();


    // ask user whether to re-do this evaluation
    bool auto_mode = settings::getParameter<settings::Boolean>(parameters,"#autoMode").value;
    if(!auto_mode){
        std::string str;
        std::cout << "Do you want to execute the same evaluation again ? Type y,Y or yes if you do, nothing otherwise" << std::endl;
        std::getline(std::cin,str);
        if(str == "y" || str == "Y" || str == "yes"){
            return true;
        }
    }

    nbrEval++;
    if(ea->update(environment)){
        nbrEval = 0;
    }

    if(sim_mode){
        ea->set_objectives(objs);
        ea->set_trajectory(traj);
    }

    ea->epoch();

    write_data();
    save_logs();

    list_ids.clear();
    choice_of_robot();

    return true;
}


