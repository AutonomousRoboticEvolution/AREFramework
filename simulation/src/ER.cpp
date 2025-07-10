#include "simulatedER/ER.h"

using namespace are;
using namespace are::sim;
namespace interproc = boost::interprocess;


/// Initialize the settings class; it will read a settings file or it will use default parameters if it cannot read a
/// settings file. A random number class will also be created and all other files refer to this class
void ER::initialize()
{

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
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




void ER::init_individual(const std::string &serialized_ind){
    if(serialized_ind.empty()){
        std::cerr << "No individual received" << std::endl;
        return;
    }
    environment->clear_data();
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
