#include "simulatedER/ER.h"

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

    if(instance_type == settings::INSTANCE_SERVER){
        //setup zmq communication channel to send individual
        std::string port = settings::getParameter<settings::String>(parameters,"#port").value;
        _individual_channel.bind("tcp://*:"+ port + "1");
    }

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

void ER::initIndividual(){
    if(ind_received){
        currentInd->init();
        return;
    }
    std::string message;
    receive_string_no_reply(message,_individual_channel,"ind ");
    std::cout << "received individual" << std::endl;
    ind_received = true;
    


    //int length;
    //simChar* message = simGetStringSignal("currentInd",&length);
    if(message.empty()){
        std::cerr << "No individual received" << std::endl;
        return;
    }
    
//    std::string mess(message);
//    mess.resize(length);
    currentInd = ea->get_population()[0];
    if(nbrEval == 0)
        currentInd->from_string(message);
    currentInd->init();
    
    int ind_id = currentInd->get_morph_genome()->id();
    evalIsFinish = false;
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
//    simReleaseBuffer(message);
}

void ER::handleSimulation()
{
    int instance_type =
            settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    /* This function is called every simulation step. Note that the behavior of
    * the robot drastically changes when slowing down the simulation since this
    * function will be called more often. All simulated individuals will be
    * updated until the maximum simulation time, as specified in the environment
    * class, is reached.
    */
    if (instance_type == settings::INSTANCE_DEBUGGING) {
        simStopSimulation();
        return;
    }

    simulationTime = simGetSimulationTime();
    //    if(instance_type == settings::INSTANCE_SERVER)
    //        simSetFloatSignal("simulationTime",simulationTime);


    currentInd->update(simulationTime);
    environment->updateEnv(simulationTime,std::dynamic_pointer_cast<Morphology>(currentInd->get_morphology()));
    //std::vector<double> instant_reward = environment->fitnessFunction(currentInd);
  //  currentInd->set_instant_reward(instant_reward);
    if (simulationTime >
        settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value ||
        ea->finish_eval(environment)) {
        simStopSimulation();
    }
}

void ER::endOfSimulation()
{

    int instanceType = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if(verbose)
        std::cout << "individual " << currentIndIndex << " is evaluated" << std::endl;

    nbrEval++;

    if(instanceType == settings::INSTANCE_REGULAR){
        if(currentIndIndex < ea->get_population().size())
        {
            std::vector<double> objectives = environment->fitnessFunction(currentInd);
            if(verbose){
                std::cout << "fitnesses = " << std::endl;
                for(const double fitness : objectives)
                    std::cout << fitness << std::endl;
            }
            ea->setObjectives(currentIndIndex,objectives);
            std::cout << environment << std::endl;

            end_eval_time = hr_clock::now();
            std::stringstream sstr;
            sstr << "eval," << std::chrono::duration_cast<std::chrono::microseconds>(start_eval_time - reference_time).count()
                 << "," << std::chrono::duration_cast<std::chrono::microseconds>(end_eval_time - reference_time).count() << std::endl;
            Logging::saveStringToFile("times.csv",sstr.str());
            if(ea->update(environment)){
                currentIndIndex++;
                nbrEval = 0;
                saveLogs(false);
            }
            else return;

        }

        if(currentIndIndex >= ea->get_population().size())
        {
            start_overhead_time = hr_clock::now();
            ea->epoch();
            saveLogs();
            ea->init_next_pop();
            if(verbose)
                std::cout << "-_- GENERATION _-_ " << ea->get_generation() << " finished" << std::endl;
            ea->incr_generation();
            currentIndIndex = 0;
            end_overhead_time = hr_clock::now();
            std::stringstream sstr;
            sstr << "overhead," << std::chrono::duration_cast<std::chrono::microseconds>(start_overhead_time - reference_time).count()
                 << "," << std::chrono::duration_cast<std::chrono::microseconds>(end_overhead_time - reference_time).count() << std::endl;
            Logging::saveStringToFile("times.csv",sstr.str());
        }

        if(ea->is_finish()){
            saveEndLogs();
            if(verbose)
            {
                std::cout << "---------------------" << std::endl;
                std::cout << "Evolution is Finished" << std::endl;
                std::cout << "---------------------" << std::endl;
            }
            simQuitSimulator(true);
            return;
        }
    }
    else if(instanceType == settings::INSTANCE_SERVER){
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
        simSetIntegerSignal("evalIsFinish",(simInt)evalIsFinish);
    }
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
