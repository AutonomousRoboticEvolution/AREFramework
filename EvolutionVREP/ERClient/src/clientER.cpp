#include "ERClient/clientER.h"

using namespace are::client;

int ER::init(int nbrOfInst, int port){
    for (int i = 0; i < nbrOfInst; i++) {
        auto new_slave = std::unique_ptr<SlaveConnection>(new SlaveConnection("127.0.0.1", i + port));
        std::cout << "Connecting to vrep on port " << new_slave->port() << std::endl;
        if (new_slave->connect(5000)) {
            // new_slave->setState(SlaveConnection::State::FREE);
            new_slave->getIntegerSignalStreaming("simulationState");
            serverInstances.push_back(std::move(new_slave));

        } else {
            std::cerr << "Could not connect to V-REP on port " << new_slave->port() << std::endl;
            if (settings::getParameter<settings::Boolean>(parameters,"#killWhenNotConnected").value) {
                return false;
            }
            continue; // jump back to beginning loop
        }
    }
    initialize();
    return true;
}

void ER::initialize(){

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;

    Logging::create_log_folder(repository + std::string("/") + exp_name);

    if(!load_fct_exp_plugin<Logging::Factory>
            (loggingFactory,exp_plugin_name,"loggingFactory"))
        exit(1);

    loggingFactory(logs,parameters);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,exp_plugin_name,"EAFactory"))
        exit(1);

    ea = EAFactory(randNum, parameters);
    ea->init();
}

bool ER::execute()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    bool shouldReopenConnections = settings::getParameter<settings::Boolean>(parameters,"#shouldReopenConnections").value;
    int tries = 0;
    int pauseTime = 100; // milliseconds
    // communicate with all ports
//    if (verbose) {
//        std::cout << "number server instances = " << serverInstances.size() << std::endl;
//    }
    if (shouldReopenConnections) {
        reopenConnections();
    }

    //    bool doneEvaluating = false;
    if (!confirmConnections()) {
        return false;
    }
//    else {
//        tries = 0;
//    }
    updateSimulation();
//    while(serverInstances.size() > 0)
//    {
//        if (doneEvaluating) {
//            break;
//        }

//        updateSimulation();
//    }
    return true;
}

void ER::startOfSimulation(){
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Simulation" << std::endl;

    currentInd = ea->getIndividual(currentIndIndex);
    currentInd->set_properties(properties);
}

void ER::endOfSimulation(){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int nbrOfGen = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;
    if(verbose)
        std::cout << "individual " << currentIndIndex << " is evaluated" << std::endl;


    if(currentIndIndex < ea->get_population().size())
    {
        float fitness = currentInd->getFitness();
        if(verbose)
            std::cout << "fitness = " << fitness << std::endl;
        ea->setFitness(currentIndIndex,fitness);
        currentIndIndex++;
    	saveLogs(false);
    }
    if(currentIndIndex >= ea->get_population().size())
    {
        saveLogs();
        ea->epoch();
        if(verbose)
            std::cout << "generation " << ea->get_generation() << " finished" << std::endl;
        ea->incr_generation();
        currentIndIndex = 0;

    }
    if(ea->get_generation() >= nbrOfGen){
        std::cout << "---------------------" << std::endl;
        std::cout << "Evolution is Finished" << std::endl;
        std::cout << "---------------------" << std::endl;
    }
}

void ER::updateSimulation()
{
    for(const auto &slave : serverInstances)
    {
        int state = slave->getIntegerSignal("simulationState");

//        std::cout << "CLIENT " << slave->get_clientID() << " spin" << std::endl;

        if(state == IDLE)
        {
            slave->setIntegerSignal("clientState",IDLE);
            return;
        }
        else if(state == READY)
        {
            ///@todo start in slave to handle errors
//            simxStartSimulation(slave->get_clientID(),simx_opmode_blocking);
            startOfSimulation();
            currentInd->set_client_id(slave->get_clientID());
            slave->setStringSignal("currentInd",currentInd->to_string());
            slave->setIntegerSignal("clientState",READY);
        }
        else if(state == BUSY)
        {
            float simTime = slave->getFloatSignal("simulationTime");
            slave->setIntegerSignal("clientState",BUSY);

        }
        else if(state == FINISH)
        {
            std::string message;
            slave->getStringSignal("currentInd",message);
            currentInd->from_string(message);
            endOfSimulation();
            slave->setIntegerSignal("clientState",IDLE);
        }
        else if(state == ERROR)
        {
            std::cerr << "An error happened on the server side" << std::endl;
        }
        else
            std::cerr << "state value unknown : " << state << std::endl
                      << "Possible states are : " << std::endl
                      << "\t IDLE : 0" << std::endl
                      << "\t READY : 1" << std::endl
                      << "\t BUSY : 2" << std::endl
                      << "\t FINISH : 3" << std::endl
                      << "\t ERROR : 9" << std::endl;
    }
}

void ER::quitSimulation()
{
    for (auto &slave: serverInstances) {
        std::cout << "closing simulator " << slave->port() << std::endl;
        slave->setIntegerSignal("clientState", 99);
        slave->disconnect();
    }
    serverInstances.clear();
}

void ER::finishConnections()
{
    for (auto &slave: serverInstances) {
        slave->disconnect();
    }
    extApi_sleepMs(100);
}

void ER::openConnections()
{
    for (auto &slave: serverInstances) {
        std::cout << "Reconnecting to vrep on port " << slave->port() << std::endl;
        slave->connect(5000);
        slave->getIntegerSignalStreaming("simulationState");
    }
    extApi_sleepMs(100);
}

void ER::reopenConnections()
{
    finishConnections();
    openConnections();
}

bool ER::confirmConnections()
{
    int tries = 0;
    for (auto &slave: serverInstances) {
        if (!slave->status()) {
            std::cerr << "Client could not connect to server in port, trying again " << slave->port() << std::endl;
            tries++;
            extApi_sleepMs(20);
        }
        else {
            continue;
        }
        if (tries > loadingTrials) {
        std::cerr << "One V-REP instance is faulty since I tried to connect to it for more than 1000 times." << std::endl;
            return false;
        }
    }

//    std::cout << "Connections confirmed" << std::endl;
    return true;
}
