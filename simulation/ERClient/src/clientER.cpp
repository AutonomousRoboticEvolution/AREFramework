#include "ERClient/clientER.h"

using namespace are::client;

int ER::init(int nbrOfInst, int port){
    initialize();
    nbr_of_instances = nbrOfInst;
    for (int i = 0; i < nbrOfInst; i++) {
        auto new_slave = std::unique_ptr<SlaveConnection>(new SlaveConnection("127.0.0.1", i + port));
        std::cout << "Connecting to vrep on port " << new_slave->port() << std::endl;
        if (new_slave->connect(5000)) {
            // new_slave->setState(SlaveConnection::State::FREE);
           // while(new_slave->getIntegerSignalStreaming("simulationState")!=0);
          //  new_slave->setIntegerSignal("clientState",IDLE);
            new_slave->setStringSignal("log_folder",Logging::log_folder),
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
    eval_times.resize(serverInstances.size());
    return true;
}

void ER::initialize(){

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;
    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;

    Logging::create_log_folder(repository + std::string("/") + exp_name);
    std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(exp_plugin_name);

    if(!load_fct_exp_plugin<Logging::Factory>
            (loggingFactory,libhandler,"loggingFactory"))
        exit(1);

    loggingFactory(logs,parameters);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,libhandler,"EAFactory"))
        exit(1);

    ea = EAFactory(randNum, parameters);
    ea->set_simulator_side(false);
    ea->init();
    population_size = ea->get_population().size();
    for(int i = 0; i < ea->get_population().size(); i++)
        indToEval.push_back(i);
}

bool ER::execute()
{
    bool shouldReopenConnections = settings::getParameter<settings::Boolean>(parameters,"#shouldReopenConnections").value;
    bool update_sim_list = settings::getParameter<settings::Boolean>(parameters,"#updateSimulatorList").value;

    if (shouldReopenConnections) {
        reopenConnections();
    }

    confirmConnections();
    if(update_sim_list)
        serverInstances = updateSimulatorList();
    if(serverInstances.empty())
        return false;


    return updateSimulation();
}

void ER::startOfSimulation(int slaveIndex){
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Simulation" << std::endl;

    int eval_order = settings::getParameter<settings::Integer>(parameters,"#evaluationOrder").value;
    if(eval_order == EvalOrder::FIFO){
        //First in Last out
        currentIndVec[slaveIndex] = ea->getIndividual(indToEval.back());
        currentIndexVec[slaveIndex] = indToEval.back();
        indToEval.erase(indToEval.begin()+indToEval.size());
    }
    else if(eval_order == EvalOrder::FILO){
        //First in First out
        currentIndVec[slaveIndex] = ea->getIndividual(indToEval.front());
        currentIndexVec[slaveIndex] = indToEval.front();
        indToEval.erase(indToEval.begin());
    }
    else if(eval_order == EvalOrder::RANDOM){
        //Random pick
        int rand_idx = randNum->randInt(0,indToEval.size()-1);
        int index = indToEval[rand_idx];
        currentIndVec[slaveIndex] = ea->getIndividual(index);
        currentIndexVec[slaveIndex] = index;
        indToEval.erase(indToEval.begin()+rand_idx);
        indToEval.shrink_to_fit();
    }
    else if(eval_order == EvalOrder::FIFO_RANDOM){
        std::vector<int> discrete_distri;
        for(int i = 0; i < indToEval.size(); i++)
            discrete_distri.push_back(i);

        std::discrete_distribution<int> dd(discrete_distri.begin(),discrete_distri.end());
        //Random pick
        int rand_idx = dd(randNum->gen);
        int index = indToEval[rand_idx];
        currentIndVec[slaveIndex] = ea->getIndividual(index);
        if(currentIndVec[slaveIndex] == nullptr)
            currentIndexVec[slaveIndex] = -1;
        else
            currentIndexVec[slaveIndex] = index;
        indToEval.erase(indToEval.begin()+rand_idx);
        indToEval.shrink_to_fit();
    }
    if(currentIndexVec[slaveIndex] < 0)
        return;
    currentIndVec[slaveIndex]->set_client_id(serverInstances[slaveIndex]->get_clientID());
    currentIndVec[slaveIndex]->set_individual_id(currentIndexVec[slaveIndex]);
    currentIndVec[slaveIndex]->set_generation(ea->get_generation());
    serverInstances[slaveIndex]->setStringSignal("currentInd",currentIndVec[slaveIndex]->to_string());
    serverInstances[slaveIndex]->setIntegerSignal("clientState",READY);
}

void ER::endOfSimulation(int slaveIndex){
    if(currentIndexVec[slaveIndex] < 0)
        return;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    std::string message;
    serverInstances[slaveIndex]->getStringSignal("currentInd",message);
    try{
        currentIndVec[slaveIndex]->from_string(message);
    }catch(boost::archive::archive_exception& e){
        std::cerr << e.what() << std::endl;
        return;
    }

    evalIsFinish = serverInstances[slaveIndex]->getIntegerSignal("evalIsFinish");

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
    ea->update(environment);
    if(population_size < ea->get_pop_size() && !ea->is_finish()){
        for(int i = population_size; i < ea->get_pop_size(); i++)
            indToEval.push_back(i);
        population_size = ea->get_pop_size();
    }

    //        if(evalIsFinish)
    //            currentIndIndex++;

    saveLogs(false);
}

bool ER::updateSimulation()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    bool all_instances_finish = true;
    int state = IDLE;

    if(ea->get_population().size() > 0){
        for(size_t slaveIdx = 0; slaveIdx < serverInstances.size(); slaveIdx++)
        {
            state = serverInstances[slaveIdx]->getIntegerSignal("simulationState");
            all_instances_finish = all_instances_finish && state == READY && indToEval.empty();

            //        std::cout << "CLIENT " << slave->get_clientID() << " spin" << std::endl;

            if(state == IDLE)
            {
                serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
            }
            else if(state == READY && !indToEval.empty())
            {
                eval_times[slaveIdx].first = hr_clock::now();

                ///@todo start in slave to handle errors
                //            simxStartSimulation(slave->get_clientID(),simx_opmode_blocking);
                startOfSimulation(slaveIdx);
            }
            else if(state == BUSY)
            {
                continue;
                //            float simTime = slave->getFloatSignal("simulationTime");
                //            serverInstances[slaveIdx]->setIntegerSignal("clientState",BUSY);
            }
            else if(state == RESTART){
                currentIndVec[slaveIdx]->set_client_id(serverInstances[slaveIdx]->get_clientID());
                serverInstances[slaveIdx]->setStringSignal("currentInd",currentIndVec[slaveIdx]->to_string());
                serverInstances[slaveIdx]->setIntegerSignal("clientState",READY);
                std::cout << "Restart simulator " << slaveIdx << std::endl;
            }
            else if(state == FINISH)
            {
                endOfSimulation(slaveIdx);
                serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
                eval_times[slaveIdx].second = hr_clock::now();
                std::stringstream sstr;
                sstr << "eval," << std::chrono::duration_cast<std::chrono::microseconds>(eval_times[slaveIdx].first - reference_time).count()
                     << "," << std::chrono::duration_cast<std::chrono::microseconds>(eval_times[slaveIdx].second - reference_time).count() << std::endl;
                Logging::saveStringToFile("times.csv",sstr.str());
            }
            else if(state == ERROR)
            {
                std::cerr << "An error happened on the server side" << std::endl;
            }
            else if(state == READY && indToEval.empty()){                
                std::cout << "Slave " << slaveIdx << " Waiting for all instances to finish before starting next generation" << std::endl;
                continue;
            }
            else{
//                std::cerr << "state value unknown : " << state << std::endl
//                          << "Possible states are : " << std::endl
//                          << "\t IDLE : 0" << std::endl
//                          << "\t READY : 1" << std::endl
//                          << "\t BUSY : 2" << std::endl
//                          << "\t FINISH : 3" << std::endl
//                          << "\t ERROR : 9" << std::endl;
                all_instances_finish = false;
            }
        }
    }

    bool wait_for_all_instances = settings::getParameter<settings::Boolean>(parameters,"#waitForAllInstances").value;
    if(!wait_for_all_instances) all_instances_finish = true;

    if(indToEval.empty() && all_instances_finish || ea->get_population().size() == 0)
    {
        start_overhead_time = hr_clock::now();
        ea->epoch();
        saveLogs();
        ea->init_next_pop();
        ea->fill_ind_to_eval(indToEval);
        if(verbose)
            std::cout << "-_- GENERATION _-_ " << ea->get_generation() << " finished" << std::endl;
        ea->incr_generation();
        end_overhead_time = hr_clock::now();
        std::stringstream sstr;
        sstr << "overhead," << std::chrono::duration_cast<std::chrono::microseconds>(start_overhead_time - reference_time).count()
             << "," << std::chrono::duration_cast<std::chrono::microseconds>(end_overhead_time - reference_time).count() << std::endl;
        Logging::saveStringToFile("times.csv",sstr.str());
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
    int counter = 0;
    for (auto &slave: serverInstances) {
        if (!slave->status()) {
            std::cerr << "Client could not connect to server in port, trying again " << slave->port() << std::endl;
            slave->incr_reconnection_trials();
            extApi_sleepMs(20);
        }
        else {
            counter++;
            continue;
        }
    }
    std::cout << counter << "/" << serverInstances.size() << " servers online" << std::endl;
    //    std::cout << "Connections confirmed" << std::endl;
    return true;
}

std::vector<std::unique_ptr<SlaveConnection>> ER::updateSimulatorList(){


    std::vector<std::unique_ptr<SlaveConnection>> newServInst;
    std::vector<Individual::Ptr> newCurrentIndVec;
    std::vector<int> newCurrentIndexVec;
    for(size_t idx = 0; idx < serverInstances.size();idx++){
        if(serverInstances[idx]->get_reconnection_trials() > loadingTrials){
            std::cerr << "One V-REP instance is faulty since I tried to connect to it for more than " << loadingTrials <<  " times." << std::endl;
            serverInstances[idx].reset();
            indToEval.push_back(currentIndexVec[idx]);
        }else {
            newServInst.push_back(std::move(serverInstances[idx]));
            newCurrentIndVec.push_back(currentIndVec[idx]);
            newCurrentIndexVec.push_back(currentIndexVec[idx]);
        }
     }


    currentIndVec = newCurrentIndVec;
    currentIndexVec = newCurrentIndexVec;

    return newServInst;
}
