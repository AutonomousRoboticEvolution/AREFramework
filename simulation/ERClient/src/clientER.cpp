#include "ERClient/clientER.h"
#include "tbb/tbb.h"

using namespace are::client;

int ER::init(int nbrOfInst, int port){
    initialize();
    nbr_of_instances = nbrOfInst;
    for (int i = 0; i < nbrOfInst; i++) {
        auto new_slave = std::make_unique<SlaveConnection>("127.0.0.1", i + port);
        std::cout << "Connecting to vrep on port " << new_slave->port() << std::endl;
        if (new_slave->connect(5000)) {
            // new_slave->setState(SlaveConnection::State::FREE);
           // while(new_slave->getIntegerSignalStreaming("simulationState")!=0);
          //  new_slave->setIntegerSignal("clientState",IDLE);
            new_slave->setStringSignal("log_folder",Logging::log_folder);
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
    currentIndexVec.resize(serverInstances.size(),-1);
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
        indToEval.push_back(ea->get_population().get_index(i));
}

bool ER::execute()
{
    bool shouldReopenConnections = settings::getParameter<settings::Boolean>(parameters,"#shouldReopenConnections").value;

    if (shouldReopenConnections) {
        reopenConnections();
    }

    confirmConnections();
    updateSimulatorList();
    if(serverInstances.empty())
        return false;


    return updateSimulation();
}

void ER::individuals_distribution(){
    for(int i = 0; i < currentIndexVec.size(); i++){
        if(indToEval.empty())
            break;
        if(currentIndexVec[i] >= 0)
            continue;
        int eval_order = settings::getParameter<settings::Integer>(parameters,"#evaluationOrder").value;
        if(eval_order == EvalOrder::FILO){
            //First in Last out
            currentIndVec[i] = ea->getIndividual(indToEval.back());
            if(currentIndVec[i] == nullptr)
                currentIndexVec[i] = -1;
            else
                currentIndexVec[i] = indToEval.back();
            indToEval.erase(indToEval.begin()+indToEval.size()-1);
        }
        else if(eval_order == EvalOrder::FIFO){
            //First in First out
            currentIndVec[i] = ea->getIndividual(indToEval.front());
            if(currentIndVec[i] == nullptr)
                currentIndexVec[i] = -1;
            else
                currentIndexVec[i] = indToEval.front();
            indToEval.erase(indToEval.begin());
        }
        else if(eval_order == EvalOrder::RANDOM){
            //Random pick
            int rand_idx = randNum->randInt(0,indToEval.size()-1);
            int index = indToEval[rand_idx];
            currentIndVec[i] = ea->getIndividual(index);
            if(currentIndVec[i] == nullptr)
                currentIndexVec[i] = -1;
            else
                currentIndexVec[i] = index;
            indToEval.erase(indToEval.begin()+rand_idx);
           // indToEval.shrink_to_fit();
        }
        else if(eval_order == EvalOrder::FIFO_RANDOM){
            std::vector<int> discrete_distri;
            for(int i = indToEval.size(); i > 0 ; i--)
                discrete_distri.push_back(i*10);

            std::discrete_distribution<int> dd(discrete_distri.begin(),discrete_distri.end());
            //Random pick
            int rand_idx = dd(randNum->gen);
            int index = indToEval[rand_idx];
            for(int idx : currentIndexVec)
                assert(idx != index);
            currentIndVec[i] = ea->getIndividual(index);
            if(currentIndVec[i] == nullptr)
                currentIndexVec[i] = -1;
            else
                currentIndexVec[i] = index;
            std::cout << indToEval.size() << " " << index << std::endl;
            indToEval.erase(indToEval.begin()+rand_idx);

            //indToEval.shrink_to_fit();
        }
    }
}

void ER::startOfSimulation(int slaveIndex){
//    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
//         std::cout << "sim " << slaveIndex << " Starting Simulation" << std::endl;

    if(currentIndexVec[slaveIndex] < 0)
        return;
    currentIndVec[slaveIndex]->set_client_id(serverInstances[slaveIndex]->get_clientID());
    currentIndVec[slaveIndex]->set_individual_id(currentIndexVec[slaveIndex]);
    currentIndVec[slaveIndex]->set_generation(ea->get_generation());
    serverInstances[slaveIndex]->setStringSignal("currentInd",currentIndVec[slaveIndex]->to_string());
    serverInstances[slaveIndex]->setIntegerSignal("clientState",READY);
}

bool ER::endOfSimulation(int slaveIndex){
    if(currentIndexVec[slaveIndex] < 0)
        return true;
    //bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    std::string message;
    int timeout_counter = 0;
    do{
        serverInstances[slaveIndex]->getStringSignal("currentInd",message);
        //try{
        if(message != "")
            currentIndVec[slaveIndex]->from_string(message);
        timeout_counter++;

        //}catch(boost::archive::archive_exception& e){
          //  std::cerr << e.what() << std::endl;
           // return false;
        //}

    }while((currentIndVec[slaveIndex]->getObjectives().empty() || message == "") && timeout_counter < 10);

    //quick and dirty fix on lost individual with the simulator. To be change to reask to the simulator the individual
    if(timeout_counter >= 10)
        currentIndVec[slaveIndex]->setObjectives({0});



    evalIsFinish = serverInstances[slaveIndex]->getIntegerSignal("evalIsFinish");

   // if(verbose)
   //     std::cout << "Slave : " << slaveIndex << " individual " << currentIndexVec[slaveIndex] << " with morphology " << currentIndVec[slaveIndex]->get_morph_genome()->id() <<" is evaluated" << std::endl;

    std::vector<double> objectives = currentIndVec[slaveIndex]->getObjectives();
   // if(verbose){
//        std::cout << "Slave : " << slaveIndex << " fitness = ";
//        for(const double obj : objectives)
//            std::cout << obj <<  ", ";
//        std::cout << std::endl;
//    }
    ea->setObjectives(currentIndexVec[slaveIndex],objectives);


    //        if(evalIsFinish)
    //            currentIndIndex++;
  //  if(evalIsFinish)
  //      saveLogs(false);
    return true;
}

std::string ER::sim_state_to_string(int state){
    std::stringstream sstr;
    if(state == IDLE)
        sstr << "IDLE";
    else if(state == READY)
        sstr << "READY";
    else if(state == BUSY)
        sstr << "BUSY";
    else if(state == RESTART)
        sstr << "RESTART";
    else if(state == FINISH)
        sstr << "FINISH";
    else if(state == ERROR)
        sstr << "ERROR";
    else sstr << "unknown state";
    return sstr.str();
}

bool ER::updateSimulation()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

  // bool all_instances_finish = true;
    int state = IDLE;
    if(!indToEval.empty())
        individuals_distribution();

    if(ea->get_population().size() > 0){
//        for(size_t slaveIdx = 0; slaveIdx < serverInstances.size(); slaveIdx++)
//        {

         tbb::parallel_for(tbb::blocked_range<size_t>(0,serverInstances.size()),
                          [&](tbb::blocked_range<size_t> r){

            for(size_t slaveIdx = r.begin(); slaveIdx != r.end(); ++slaveIdx)
            {
                serverInstances[slaveIdx]->setStringSignal("log_folder",Logging::log_folder);

                if(serverInstances[slaveIdx]->state() == SlaveConnection::DOWN)
                    continue;

                state = serverInstances[slaveIdx]->getIntegerSignal("simulationState");

               // if(verbose)
               //     std::cout << "Slave : " << slaveIdx << " " << sim_state_to_string(state) << std::endl;
               // all_instances_finish = all_instances_finish && state == READY && indToEval.empty();

                //        std::cout << "CLIENT " << slave->get_clientID() << " spin" << std::endl;

                if(state == IDLE)
                {
                    serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
                    currentIndexVec[slaveIdx] = -1;
                }
                else if(state == READY && currentIndexVec[slaveIdx] >= 0)
                {
                    eval_times[slaveIdx].first = hr_clock::now();

                    ///@todo start in slave to handle errors
                    //            simxStartSimulation(slave->get_clientID(),simx_opmode_blocking);
                    startOfSimulation(slaveIdx);
                   // currentIndexVec[slaveIdx] = -1;
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
                    if(endOfSimulation(slaveIdx)){
                        currentIndexVec[slaveIdx] = -1;
                        serverInstances[slaveIdx]->setIntegerSignal("clientState",IDLE);
                    }
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
                  //  std::cout << "Slave " << slaveIdx << " Waiting for all instances to finish before starting next generation" << std::endl;
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
                   // all_instances_finish = false;
                }
            }});
        //}
	
        ea->update(environment);
        
       	if(!indToEval.empty() && ea->get_population().get_index(ea->get_pop_size()-1) != indToEval.back() && !ea->is_finish()){
            int s = 0;
            for(;s < ea->get_pop_size(); s++)
                if(ea->get_population().get_index(s) == indToEval.back())
                    break;
            for(int i = s+1; i < ea->get_pop_size(); i++)
                indToEval.push_back(ea->get_population().get_index(i));
            population_size = ea->get_pop_size();
        }
    }
    saveLogs(false);
//    bool wait_for_all_instances = settings::getParameter<settings::Boolean>(parameters,"#waitForAllInstances").value;
//    if(!wait_for_all_instances) all_instances_finish = true;

    if(indToEval.empty() && !ea->get_population().empty() && isAllInstancesFinished()){
        ea->fill_ind_to_eval(indToEval);
        return true;
    }

    if(indToEval.empty() && isAllInstancesFinished() || ea->get_population().size() == 0)
    {
        start_overhead_time = hr_clock::now();
        ea->epoch();
        saveLogs();
        ea->init_next_pop();
        ea->fill_ind_to_eval(indToEval);
        for(int i = 0; i < indToEval.size(); i++){
            for(int j = 0; j < indToEval.size(); j++){
                assert(!(i != j && indToEval[i] == indToEval[j]));
            }
        }
        if(verbose)
            std::cout << "-_- GENERATION _-_ " << ea->get_generation() << " finished" << std::endl;
        ea->incr_generation();
        end_overhead_time = hr_clock::now();
        std::stringstream sstr;
        sstr << "overhead," << std::chrono::duration_cast<std::chrono::microseconds>(start_overhead_time - reference_time).count()
             << "," << std::chrono::duration_cast<std::chrono::microseconds>(end_overhead_time - reference_time).count() << std::endl;
        Logging::saveStringToFile("times.csv",sstr.str());

    }
    if(ea->is_finish()){
        if(verbose)
        {
            std::cout << "---------------------" << std::endl;
            std::cout << "Evolution is Finished" << std::endl;
            std::cout << "---------------------" << std::endl;
        }
        return false;
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
        //[r    std::cerr << "Client could not connect to server in port, trying again " << slave->port() << std::endl;
            slave->incr_reconnection_trials();
            extApi_sleepMs(20);
        }
        else {
            counter++;
            continue;
        }
    }
   // std::cout << counter << "/" << serverInstances.size() << " servers online" << std::endl;
    //    std::cout << "Connections confirmed" << std::endl;
    return true;
}

void ER::updateSimulatorList(){
    std::vector<std::unique_ptr<SlaveConnection>> newServInst;
    std::vector<Individual::Ptr> newCurrentIndVec;
    std::vector<int> newCurrentIndexVec;
    for(size_t idx = 0; idx < serverInstances.size();idx++){
        if(serverInstances[idx]->get_reconnection_trials() > loadingTrials){
            //std::cerr << "V-REP instance " << idx << " is faulty since I tried to connect to it for more than " << loadingTrials <<  " times." << std::endl;
            bool update_sim_list = settings::getParameter<settings::Boolean>(parameters,"#updateSimulatorList").value;
            if(update_sim_list)
                serverInstances[idx].reset();
            else
                newServInst.push_back(std::move(serverInstances[idx]));
            currentIndexVec[idx] = -1;
//  Putting the individual from the faulty simulator back. Currently commented because a particular morphology seems to crash simulators repitively.
//            bool contain = false;
//            for(int i: indToEval)
//                if(i == currentIndexVec[idx])
//                    contain = true;
//            if(!contain)
//                indToEval.insert(indToEval.begin(),currentIndexVec[idx]);
            newServInst.back()->setState(SlaveConnection::DOWN);
        }else {
            newServInst.push_back(std::move(serverInstances[idx]));
            newServInst.back()->setState(SlaveConnection::FREE);
            newCurrentIndVec.push_back(currentIndVec[idx]);
            newCurrentIndexVec.push_back(currentIndexVec[idx]);
        }
     }

    currentIndexVec = newCurrentIndexVec;
    serverInstances.clear();
    serverInstances.resize(newServInst.size());
    serverInstances.shrink_to_fit();
    for(size_t i = 0; i < newServInst.size(); i++)
        serverInstances[i] = std::move(newServInst[i]);
    currentIndVec.clear();
    currentIndVec.resize(newServInst.size());
    currentIndVec.shrink_to_fit();
    currentIndexVec.clear();
    currentIndexVec.resize(newServInst.size());
    currentIndexVec.shrink_to_fit();
    for(size_t i = 0; i < currentIndVec.size(); i++){
        if(serverInstances[i]->state() == SlaveConnection::DOWN){
            currentIndexVec[i] = -1;
            continue;
        }
        currentIndVec[i] = std::move(newCurrentIndVec.front());
        currentIndexVec[i] = newCurrentIndexVec.front();
        newCurrentIndexVec.erase(newCurrentIndexVec.begin());
        newCurrentIndVec.erase(newCurrentIndVec.begin());
    }
}

bool ER::isAllInstancesFinished(){
    bool yes = true;
    for(int index: currentIndexVec)
        yes = yes && index == -1;

    return yes;
}
