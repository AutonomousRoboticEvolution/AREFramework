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


    if (shouldReopenConnections) {
        reopenConnections();
    }

    if (!confirmConnections()) {
        return false;
    }

    return updateSimulation();
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

    std::vector<double> objectives = currentIndVec[slaveIndex]->getObjectives();
    if(verbose){
        std::cout << "Slave : " << slaveIndex << " fitness = ";
        for(const double obj : objectives)
            std::cout << obj <<  ", ";
        std::cout << std::endl;
    }
    ea->setObjectives(currentIndexVec[slaveIndex],objectives);



    //        if(evalIsFinish)
    //            currentIndIndex++;
    ea->set_endEvalTime(hr_clock::now());
    saveLogs(false);
}

bool ER::updateSimulation()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    bool all_instances_finish = true;

    for(size_t slaveIdx = 0; slaveIdx < serverInstances.size(); slaveIdx++ )
    {
        int state = serverInstances[slaveIdx]->getIntegerSignal("simulationState");
        all_instances_finish = all_instances_finish
                && (state == READY)
                && (currentIndIndex >= ea->get_population().size());

        //        std::cout << "CLIENT " << slave->get_clientID() << " spin" << std::endl;

        if(state == IDLE)
        {
            serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
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
            //serverInstances[slaveIdx]->setIntegerSignal("clientState",BUSY);

        }
        else if(state == FINISH)
        {
            std::string message;
            serverInstances[slaveIdx]->getStringSignal("currentInd",message);
            currentIndVec[slaveIdx]->from_string(message);
            evalIsFinish = serverInstances[slaveIdx]->getIntegerSignal("evalIsFinish");
            endOfSimulation(slaveIdx);
            serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
            ea->incr_nbr_eval();
        }
        else if(state == ERROR)
        {
            std::cerr << "An error happened on the server side" << std::endl;
        }
        else if(state == READY && currentIndIndex >= ea->get_population().size()){
            std::cout << "SlaveIdx : " << slaveIdx << " Waiting for all instances to finish before starting next generation" << std::endl;
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
    if(currentIndIndex >= ea->get_population().size() && all_instances_finish)
    {
        ea->epoch();
        saveLogs();
        ea->init_next_pop();
        if(verbose)
            std::cout << "-_- GENERATION _-_ " << ea->get_generation() << " finished" << std::endl;
        ea->incr_generation();
        currentIndIndex = 0;

        if(ea->is_finish()){
            if(verbose)
            {
                std::cout << "---------------------" << std::endl;
                std::cout << "Evolution is Finished" << std::endl;
                std::cout << "---------------------" << std::endl;
            }
            return false;
        }
    }
    return true;
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
