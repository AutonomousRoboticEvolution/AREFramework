#include "simulatedER/ER.h"

using namespace are;
using namespace are::sim;
namespace interproc = boost::interprocess;


/// Initialize the settings class; it will read a settings file or it will use default parameters if it cannot read a
/// settings file. A random number class will also be created and all other files refer to this class
void ER::initialize()
{

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    if(instance_type == settings::INSTANCE_REGULAR){
        std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
        std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;

        Logging::create_log_folder(repository + std::string("/") + exp_name);
    }

    if (verbose) {
        std::cout << "ER initialize" << std::endl;
    }

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;

    std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(exp_plugin_name);

    if(!load_fct_exp_plugin<VirtualEnvironment::Factory>
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

    // if(instance_type == settings::INSTANCE_SERVER){
    //     //setup zmq communication channel to send individual
    //     std::string port = settings::getParameter<settings::String>(parameters,"#port").value;
    //     _individual_channel.bind("tcp://*:"+ port + "1");
    // }

}



/// When V-REP starts, this function is called. Depending on the settings, it initializes the properties of the
/// individual of the optimization strategy chosen.
void ER::startOfSimulation()
{
    start_eval_time = hr_clock::now();

    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Simulation" << std::endl;

    environment->init();

    currentInd = ea->getIndividual(currentIndIndex);
    currentInd->init();
    int ind_id = currentInd->get_morph_genome()->id();
    // Get images?
    if(settings::getParameter<settings::Boolean>(parameters,"#isScreenshotEnable").value) {
        std::string image_repo = settings::getParameter<settings::String>(parameters, "#imageRepository").value;
        bool images_in_logs = settings::getParameter<settings::Boolean>(parameters, "#imagesInLogFolder").value;
        if(images_in_logs){
            Logging::create_folder(Logging::log_folder + "/" + image_repo);
            robotScreenshot(ind_id,Logging::log_folder + "/" + image_repo);
        }
        else{
            Logging::create_folder(image_repo);
            robotScreenshot(ind_id,image_repo);
        }
    }
    ea->setCurrentIndIndex(currentIndIndex);
}


void ER::init_individual(const std::string &serialized_ind){
    if(serialized_ind.empty()){
        std::cerr << "No individual received" << std::endl;
        return;
    }
    
//    std::string mess(message);
//    mess.resize(length);
    currentInd = ea->get_population()[0];
    if(nbrEval == 0)
        currentInd->from_string(serialized_ind);
    currentInd->init();
    
    evalIsFinish = false;
    if(settings::getParameter<settings::Boolean>(parameters,"#isScreenshotEnable").value) {
        int ind_id = currentInd->get_morph_genome()->id();
        std::string image_repo = settings::getParameter<settings::String>(parameters, "#imageRepository").value;
        bool images_in_logs = settings::getParameter<settings::Boolean>(parameters, "#imagesInLogFolder").value;
        if(images_in_logs){
            Logging::create_folder(Logging::log_folder + "/" + image_repo);
            robotScreenshot(ind_id,Logging::log_folder + "/" + image_repo);
        }
        else{
            Logging::create_folder(image_repo);
            robotScreenshot(ind_id,image_repo);
        }
    }
}
void ER::handleSimulation()
{
    if(currentInd.get() == nullptr)
        return;


    simulationTime = simGetSimulationTime();
    //    if(instance_type == settings::INSTANCE_SERVER)
    //        simSetFloatSignal("simulationTime",simulationTime);

    //warm up time
    float time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    if(simulationTime < 5*time_step)
        return;

    currentInd->update(simulationTime);
    environment->updateEnv(simulationTime,std::dynamic_pointer_cast<Morphology>(currentInd->get_morphology()));
    //std::vector<double> instant_reward = environment->fitnessFunction(currentInd);
  //  currentInd->set_instant_reward(instant_reward);
    if (ea->finish_eval(environment)) {
        std::cout << "STOP SIMULATION EARLY!!!" << std::endl;
        simStopSimulation();
    }
}

void ER::endOfSimulation()
{

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if(verbose)
        std::cout << "individual " << currentIndIndex << " is evaluated" << std::endl;

    nbrEval++;

    std::vector<double> objectives = environment->fitnessFunction(currentInd);
    if(verbose){
        std::cout << "fitnesses = " << std::endl;
        for(const double fitness : objectives)
            std::cout << fitness << std::endl;
    }
    ea->setObjectives(currentIndIndex,objectives);
    evalIsFinish = ea->update(environment);
    if(evalIsFinish){
        nbrEval = 0;
        ind_received = false;
    }
    simSetInt32Signal("evalIsFinish",evalIsFinish);

}

void ER::saveLogs(bool endOfGen)
{
    for(const auto &log : logs){
        if(log->isEndOfGen() == endOfGen && !log->isEndOfRun()){
            log->saveLog(ea);
        }
    }
}

void ER::saveEndLogs(){
    for(const auto &log : logs){
        if(log->isEndOfRun()){
            log->saveLog(ea);
        }
    }
}
