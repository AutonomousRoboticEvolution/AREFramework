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
    currentIndVec.resize(serverInstances.size());
    currentIndexVec.resize(serverInstances.size());
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

void ER::startOfSimulation(int slaveIndex){
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Simulation" << std::endl;

    currentIndVec[slaveIndex] = ea->getIndividual(currentIndIndex);
    currentIndexVec[slaveIndex] = currentIndIndex;
    currentIndVec[slaveIndex]->set_properties(properties);
}

void ER::endOfSimulation(int slaveIndex){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(verbose)
        std::cout << "Slave : " << slaveIndex << " individual " << currentIndexVec[slaveIndex] << " is evaluated" << std::endl;

    float fitness = currentIndVec[slaveIndex]->getFitness();
    if(verbose)
        std::cout << "Slave : " << slaveIndex << " fitness = " << fitness << std::endl;
    ea->setFitness(currentIndexVec[slaveIndex],fitness);
    //        if(evalIsFinish)
    //            currentIndIndex++;
    ea->set_endEvalTime(hr_clock::now());
    saveLogs(false);
}

void ER::updateSimulation()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int nbrOfGen = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;

    bool all_instances_finish = true;

    for(size_t slaveIdx = 0; slaveIdx < serverInstances.size(); slaveIdx++ )
    {
        int state = serverInstances[slaveIdx]->getIntegerSignal("simulationState");

//        std::cout << "CLIENT " << slave->get_clientID() << " spin" << std::endl;

        if(state == IDLE)
        {
            serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
            return;
        }
        else if(state == READY && currentIndIndex < ea->get_population().size())
        {
            ///@todo start in slave to handle errors
//            simxStartSimulation(slave->get_clientID(),simx_opmode_blocking);
            ea->set_startEvalTime(hr_clock::now());
            startOfSimulation(slaveIdx);
            currentIndVec[slaveIdx]->set_client_id(serverInstances[slaveIdx]->get_clientID());
            serverInstances[slaveIdx]->setStringSignal("currentInd",currentIndVec[slaveIdx]->to_string());
            serverInstances[slaveIdx]->setIntegerSignal("clientState",READY);
            currentIndIndex++;
        }
        else if(state == BUSY)
        {
//            float simTime = slave->getFloatSignal("simulationTime");
            serverInstances[slaveIdx]->setIntegerSignal("clientState",BUSY);

        }
        else if(state == FINISH)
        {
            std::string message;
            serverInstances[slaveIdx]->getStringSignal("currentInd",message);
            currentIndVec[slaveIdx]->from_string(message);
            evalIsFinish = serverInstances[slaveIdx]->getIntegerSignal("evalIsFinish");
            endOfSimulation(slaveIdx);
            serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
        }
        else if(state == ERROR)
        {
            std::cerr << "An error happened on the server side" << std::endl;
        }
	else if(state == READY && currentIndIndex >= ea->get_population().size()){
        std::cout << "SlaveIdx : " << slaveIdx << " Waiting for all instances to finish before starting a new generation" << std::endl;
	}
        else
            std::cerr << "state value unknown : " << state << std::endl
                      << "Possible states are : " << std::endl
                      << "\t IDLE : 0" << std::endl
                      << "\t READY : 1" << std::endl
                      << "\t BUSY : 2" << std::endl
                      << "\t FINISH : 3" << std::endl
                      << "\t ERROR : 9" << std::endl;
    	all_instances_finish = all_instances_finish && (state == READY);
    }
    if(currentIndIndex >= ea->get_population().size() && all_instances_finish)
    {
        saveLogs();
        ea->epoch();
        if(verbose)
            std::cout << "-_- GENERATION _-_ " << ea->get_generation() << " finished" << std::endl;
        ea->incr_generation();
        currentIndIndex = 0;
    }
    if(ea->get_generation() >= nbrOfGen && all_instances_finish){
        if(verbose)
        {
            std::cout << "---------------------" << std::endl;
            std::cout << "Evolution is Finished" << std::endl;
            std::cout << "---------------------" << std::endl;
        }
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
