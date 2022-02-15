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
}


/// When V-REP starts, this function is called. Depending on the settings, it initializes the properties of the
/// individual of the optimization strategy chosen.
void ER::startOfSimulation()
{
    ea->set_startEvalTime(hr_clock::now());

    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Simulation" << std::endl;

    environment->init();

    currentInd = ea->getIndividual(currentIndIndex);
    currentInd->init();
    // Get images?
    if(settings::getParameter<settings::Boolean>(parameters,"#isScreenshotEnable").value) {
        std::string image_repo = settings::getParameter<settings::String>(parameters, "#imageRepository").value;
        bool images_in_logs = settings::getParameter<settings::Boolean>(parameters, "#imagesInLogFolder").value;
        if(images_in_logs){
            Logging::create_folder(Logging::log_folder + "/" + image_repo);
            robotScreenshot(currentIndIndex,ea->get_generation(),Logging::log_folder + "/" + image_repo);
        }
        else{
            Logging::create_folder(image_repo);
            robotScreenshot(currentIndIndex,ea->get_generation(),image_repo);
        }
    }
    ea->setCurrentIndIndex(currentIndIndex);
}

void ER::initIndividual(){
    ea->set_startEvalTime(hr_clock::now());

    int length;
    std::string mess(simGetStringSignal("currentInd",&length));
    if(length == 0){
        std::cerr << "No individual received" << std::endl;
        return;
    }
    mess.resize(length);
    currentInd = ea->getIndividual(0);
    if(nbrEval == 0)
          currentInd->from_string(mess);
    currentInd->init();
    evalIsFinish = false;
    if(settings::getParameter<settings::Boolean>(parameters,"#isScreenshotEnable").value) {
        std::string image_repo = settings::getParameter<settings::String>(parameters, "#imageRepository").value;
        bool images_in_logs = settings::getParameter<settings::Boolean>(parameters, "#imagesInLogFolder").value;
        if(images_in_logs){
            Logging::create_folder(Logging::log_folder + "/" + image_repo);
            robotScreenshot(currentIndIndex,ea->get_generation(),Logging::log_folder + "/" + image_repo);
        }
        else{
            Logging::create_folder(image_repo);
            robotScreenshot(currentIndIndex,ea->get_generation(),image_repo);
        }
    }
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
            if(ea->update(environment)){
                currentIndIndex++;
                nbrEval = 0;
            }
            ea->set_endEvalTime(hr_clock::now());
            saveLogs(false);
        }

        if(currentIndIndex >= ea->get_population().size())
        {
            ea->epoch();
            saveLogs();
            ea->init_next_pop();
            if(verbose)
                std::cout << "-_- GENERATION _-_ " << ea->get_generation() << " finished" << std::endl;
            ea->incr_generation();
            currentIndIndex = 0;
        }
        if(ea->is_finish()){
            if(verbose)
            {
                std::cout << "---------------------" << std::endl;
                std::cout << "Evolution is Finished" << std::endl;
                std::cout << "---------------------" << std::endl;
            }
            exit(0);
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
        if(evalIsFinish)
            nbrEval = 0;

        ea->set_endEvalTime(hr_clock::now());
        simSetIntegerSignal("evalIsFinish",(simInt)evalIsFinish);
    }
}

void ER::saveLogs(bool endOfGen)
{
    for(const auto &log : logs){
        if(log->isEndOfGen() == endOfGen){
            log->saveLog(ea);
        }
    }
}
