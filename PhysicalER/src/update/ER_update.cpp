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
    randNum.reset(new are::misc::RandNum(seed));

    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;

    Logging::create_folder(repository + std::string("/") + exp_name);
    Logging::create_folder(repository + std::string("/") + exp_name + std::string("/waiting_to_be_built/"));
    Logging::set_log_folder(repository + std::string("/") + exp_name);

    if (verbose) {
        std::cout << "ER initialize" << std::endl;
    }

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;

    std::vector<std::string> split_str;
    misc::split_line(exp_plugin_name,".",split_str);

    std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(split_str[0] + ".so");

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

    //Load list of robot's ids to be evaluated and ask user which one want to be evaluated.
    repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    load_ids_to_be_evaluated(repository + "/" + exp_name,list_ids);
    current_id = choice_of_robot_to_evaluate(list_ids);
    ea->setCurrentIndIndex(current_id);
    //-

    if(!load_fct_exp_plugin<Logging::Factory>
            (loggingFactory,libhandler,"loggingFactory"))
        exit(1);
    loggingFactory(logs,parameters);

    libhandler->close();

    if (verbose) std::cout << "ER initialized" << std::endl;
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
        std::cout << "Press Enter when the robot is ready" << std::endl;
        std::cin.ignore();
        std::cin.ignore();

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


    if(verbose) std::cout << "Starting Evaluation for robot with ID: "<<current_id<<"\n=====" << std::endl;

    eval_t1 = std::chrono::steady_clock::now();

    if(!isEnvInit){
        isEnvInit=true;
        environment->init();
    }

    // get the pi's IP address and the list of organs from the list_of_organs file
    std::string pi_address, list_of_organs;
    load_list_of_organs(repository + "/" + exp_name,current_id,pi_address,list_of_organs);

    //start ZMQ
    std::stringstream sstream1,sstream2;
    sstream1 << "tcp://" << pi_address << ":5556";
    sstream2 << "tcp://" << pi_address << ":5555";
    request.connect (sstream1.str().c_str());
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
    std::cout << ctrl_gen << std::endl;
    send_string(reply,ctrl_gen,request,"pi ");
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
            Logging::saveStringToFile( "log_file" , message );
        }
    }

    if(verbose){
        Individual::Ptr ind;
        auto objectives = environment->fitnessFunction(ind);
        std::cout << "fitnesses = " << std::endl;
        for(const double fitness : objectives)
            std::cout << fitness << std::endl;

    }

    std::string str;
    std::cout << "Do you want to execute the same evaluation again ? (y,Y,yes)" << std::endl;
    std::getline(std::cin,str);

    if(str == "y" || str == "Y" || str == "yes"){
        return true;
    }

    nbrEval++;
    if(ea->update(environment)){
        nbrEval = 0;
    }
    ea->set_endEvalTime(hr_clock::now());
    write_data();
    save_logs();


    current_id = choice_of_robot_to_evaluate(list_ids);
    ea->setCurrentIndIndex(current_id);

    return true;
}


