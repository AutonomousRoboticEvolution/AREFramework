#include "physicalER/ER.hpp"

using namespace are::phy;

void ER::initialize(){

    std::string pi_address = settings::getParameter<settings::String>(parameters,"#piAddress").value;

    std::stringstream sstream1,sstream2;
    sstream1 << "tcp://" << pi_address << ":5556";
    sstream2 << "tcp://" << pi_address << ":5555";

    //ZMQ's communication socket
//    request = zmq::socket_t(context, ZMQ_REQ);
    request.connect (sstream1.str().c_str());//TODO parametrize ip address

//    subscriber = zmq::socket_t(context,ZMQ_SUB);
    subscriber.connect(sstream2.str().c_str());
    subscriber.setsockopt(ZMQ_SUBSCRIBE,"pi ",3);

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
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

}

void ER::load_data(bool is_update){
    if(is_update){
        //load the bus addresses
        ea->load_data_for_update();
    }else{
        ea->load_data_for_generate();
    }
}

void ER::write_data(bool is_update){
    if(is_update){
        ea->write_data_for_update();
    }else{
        ea->write_data_for_generate();
    }
}

void ER::generate(){
    ea->epoch();
    ea->init_next_pop();
}

bool ER::execute(){

    if(robot_state == READY){
        std::cout << "Press Enter when the robot is ready" << std::endl;
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
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Evaluation\n=====" << std::endl;

    eval_t1 = std::chrono::steady_clock::now();

    if(!isEnvInit){
        isEnvInit=true;
        environment->init();
    }

    currentInd = ea->getIndividual(currentIndIndex);
    currentInd->init();

//    if(!send_order(are::phy::OrderType::LAUNCH,request))
//        exit(1);

    std::string reply;
    std::string param = settings::toString(*parameters.get());
    send_string(reply,param,request);
    assert(reply == "parameters_received");

    //get the addresses
    send_string(reply,param,request);
    assert(reply == "organ_addresses_received");

    std::string ctrl_gen = currentInd->get_ctrl_genome()->to_string();
    send_string(reply,ctrl_gen,request);
    assert(reply == "starting");

    ea->setCurrentIndIndex(currentIndIndex);
}

bool ER::update_evaluation(){
    float eval_time = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;

    using namespace std::chrono;

    eval_t2 = steady_clock::now();
    duration<float> eval_duration = duration_cast<duration<float>>(eval_t2 - eval_t1);

    //currentInd->update(eval_duration.count());
    environment->update_info();

    std::string message;
    wait_for_message(message,subscriber);
    message.erase(0,message.find(" ")+1);
    //std::cout << message << std::endl;

    return  /*eval_time <= eval_duration.count() ||*/ message=="finish";

}

bool ER::stop_evaluation(){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if(verbose)
        std::cout << "individual " << currentIndIndex << " is evaluated" << std::endl;


    std::string str;
    std::cout << "Do you want to execute the same evaluation again ? (y,Y,yes)" << std::endl;
    std::getline(std::cin,str);

    if(str == "y" || str == "Y" || str == "yes"){
        return true;
    }

    nbrEval++;

    if(currentIndIndex < ea->get_population().size())
    {
        std::vector<double> objectives = environment->fitnessFunction(currentInd);
        if(verbose){
            std::cout << "fitnesses = " << std::endl;
            for(const double fitness : objectives)
                std::cout << fitness << std::endl;
        }
        ea->setObjectives(currentIndIndex,objectives);

        if(ea->update(environment)){
            currentIndIndex++;
            nbrEval = 0;
        }
        ea->set_endEvalTime(hr_clock::now());
        save_logs(false);
    }

    if(currentIndIndex >= ea->get_population().size())
    {
        if(verbose)
        {
            std::cout << "---------------------" << std::endl;
            std::cout << "Update is Finished" << std::endl;
            std::cout << "---------------------" << std::endl;
        }
        return false;
    }
    return true;
}

void ER::save_logs(bool endOfGen)
{
    for(const auto &log : logs){
        if(log->isEndOfGen() == endOfGen){
            log->saveLog(ea);
        }
    }
}
