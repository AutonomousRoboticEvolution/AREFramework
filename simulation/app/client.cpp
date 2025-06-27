#include "client.hpp"
#include <dlibxx.hxx>
#include <tbb/parallel_for.h>

using namespace are;
using namespace are::sim;

bool Client::init(int nbr_instances, int port){
    initialize();
    std::cout << "seed is " << _rand_num->getSeed() << std::endl;
    _nbr_of_instances = nbr_instances;
    float max_eval_time = settings::getParameter<settings::Float>(_parameters,"#maxEvalTime").value;
    int env_number = settings::getParameter<settings::Integer>(_parameters,"#envNumber").value;
    int timeout = env_number*max_eval_time*1000 + 60000;
    for (int i = 0; i < nbr_instances; i++) {
        Simulator sim;
        sim.connect("127.0.0.1",2*i+port,timeout);
        sim.call_function("getLogFolder",{Logging::log_folder});
        std::cout << "Connecting to simulator on port " << sim.port() << std::endl;
        _simulators.push_back(sim);
       // auto list_fcts = sim.get()->getScriptFunctions(sim.get()->getScript(sim.get()->scripttype_sandbox,"simARE"));

    }
    _ind_vec.resize(_simulators.size());
    _idx_vec.resize(_simulators.size(),-1);


    return true;
}

void Client::initialize(){
    std::string exp_plugin_name = settings::getParameter<settings::String>(_parameters,"#expPluginName").value;
    std::string exp_name = settings::getParameter<settings::String>(_parameters,"#experimentName").value;
    std::string repository = settings::getParameter<settings::String>(_parameters,"#repository").value;
    
    Logging::create_log_folder(repository + std::string("/") + exp_name);
    
    std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(exp_plugin_name);
    
    if(!load_fct_exp_plugin<Logging::Factory>
        (_logging_factory,libhandler,"loggingFactory"))
        exit(1);

    _logging_factory(_logging_fcts,_parameters);

    if(!load_fct_exp_plugin<EA::Factory>
        (_ea_factory,libhandler,"EAFactory"))
        exit(1);

    _ea = _ea_factory(_rand_num, _parameters);
    _ea->set_simulator_side(false);
    _ea->init();
    _population_size = _ea->get_population().size();
    for(int i = 0; i < _ea->get_population().size(); i++)
        _eval_queue.push_back(_ea->get_population().get_index(i));
}

void Client::individuals_distribution(){
    for(int i = 0; i < _idx_vec.size(); i++){
        if(_eval_queue.empty())
            break;
        if(_idx_vec[i] >= 0)
            continue;
        int eval_order = settings::getParameter<settings::Integer>(_parameters,"#evaluationOrder").value;
        if(eval_order == EvalOrder::FILO){
            //First in Last out
            _ind_vec[i] = _ea->getIndividual(_eval_queue.back());
            if(_ind_vec[i] == nullptr)
                _idx_vec[i] = -1;
            else
                _idx_vec[i] = _eval_queue.back();
            _eval_queue.erase(_eval_queue.begin()+_eval_queue.size()-1);
        }
        else if(eval_order == EvalOrder::FIFO){
            //First in First out
            _ind_vec[i] = _ea->getIndividual(_eval_queue.front());
            if(_ind_vec[i] == nullptr)
                _idx_vec[i] = -1;
            else
                _idx_vec[i] = _eval_queue.front();
            _eval_queue.erase(_eval_queue.begin());
        }
        else if(eval_order == EvalOrder::RANDOM){
            //Random pick
            int rand_idx = _rand_num->randInt(0,_eval_queue.size()-1);
            int index = _eval_queue[rand_idx];
            _ind_vec[i] = _ea->getIndividual(index);
            if(_ind_vec[i] == nullptr)
                _idx_vec[i] = -1;
            else
                _idx_vec[i] = index;
            _eval_queue.erase(_eval_queue.begin()+rand_idx);
            // _eval_queue.shrink_to_fit();
        }
        else if(eval_order == EvalOrder::FIFO_RANDOM){
            std::vector<int> discrete_distri;
            for(int i = _eval_queue.size(); i > 0 ; i--)
                discrete_distri.push_back(i*10);
            
            std::discrete_distribution<int> dd(discrete_distri.begin(),discrete_distri.end());
            //Random pick
            int rand_idx = dd(_rand_num->gen);
            int index = _eval_queue[rand_idx];
            for(int idx : _idx_vec)
                assert(idx != index);
            _ind_vec[i] = _ea->getIndividual(index);
            if(_ind_vec[i] == nullptr)
                _idx_vec[i] = -1;
            else
                _idx_vec[i] = index;
            std::cout << _eval_queue.size() << " " << index << std::endl;
            _eval_queue.erase(_eval_queue.begin()+rand_idx);
            
            //_eval_queue.shrink_to_fit();
        }
    }
}

bool Client::update_simulators(){
    bool verbose = settings::getParameter<settings::Boolean>(_parameters,"#verbose").value;
    float sim_duration = settings::getParameter<settings::Float>(_parameters,"#maxEvalTime").value;
    if(!_eval_queue.empty())
        individuals_distribution();
    
    if(_ea->get_population().size() > 0){
        tbb::parallel_for(tbb::blocked_range<size_t>(0,_simulators.size()),
                        [&](tbb::blocked_range<size_t> r){
            for(size_t sim_idx = r.begin(); sim_idx != r.end(); ++sim_idx)
            {
        // for(size_t sim_idx = 0; sim_idx < _simulators.size(); sim_idx++){
                Simulator &sim = _simulators[sim_idx];
                // std::cout << "Simulator " << sim_idx << " State : " << Simulator::state_to_string(sim.state()) << std::endl;
                // std::vector<std::string> response = sim.call_function("checkConnection",{"check"});
                // if(response.empty()){
                //     if(verbose)
                //         std::cout << "Reconnect to simulator " << sim_idx << std::endl;
                //     sim.reconnect();
                //     sim.call_function("getLogFolder",{Logging::log_folder});
                //     continue;
                // }else{
                //     std::cout << "Sim " << sim_idx << " " << response[0] << std::endl;
                // }
                if(sim.state() == sim_simulation_stopped){//simulation not running
                    if(sim.is_individual_ready()){//there is an individual ready for retrieval
                        std::vector<std::string> ret = sim.call_function("sendRobotToClient",{});
                        _ind_vec[sim_idx]->from_string(ret[0]);
                        if(verbose)
                            std::cout << "Simulator " << sim_idx << " received individual with fitness " << _ind_vec[sim_idx]->getObjectives()[0] << std::endl;
                        _ea->setObjectives(_idx_vec[sim_idx],_ind_vec[sim_idx]->getObjectives());
                        _ea->update_individual(_idx_vec[sim_idx],_ind_vec[sim_idx]);
                        _idx_vec[sim_idx] = -1;
                        sim.ready_for_new_sim();//switch state to ready for send new ind
                    }
                    else{//Simulator ready for next simulation
                        if(_idx_vec[sim_idx] < 0)
                            continue;
                        _ind_vec[sim_idx]->set_individual_id(_idx_vec[sim_idx]);
                        _ind_vec[sim_idx]->set_generation(_ea->get_generation());
                        //    serverInstances[slaveIndex]->setStringSignal("currentInd",currentIndVec[slaveIndex]->to_string());
                        //    std::cout << "slave " << slaveIndex << " send ind" << std::endl;
                        if(sim.start()){
                            sim.sim_started();
                            usleep(1000);
                            sim.call_function("spawnRobot",{_ind_vec[sim_idx]->to_string()});
                            if(verbose)
                                std::cout << "simulation " << sim_idx <<  " started" << std::endl;
                        }
                    }

                }
                // else if(sim.state() == sim_simulation_advancing_abouttostop ||
                //         sim.state() == sim_simulation_advancing_lastbeforestop){
                //     if(verbose)
                //         std::cout << "simulation " << sim_idx <<  " stopped early" << std::endl;
                //     _idx_vec[sim_idx] = -1;
                //     sim.ready_for_new_sim();
                // }
                else if(sim.state() == sim_simulation_advancing_running){
                    // std::cout << "Simulator " << sim_idx << " " << sim.time() << "/" << sim_duration << std::endl;
                    if(sim.time() >= sim_duration){
                        if(sim.stop() && verbose){
                            std::cout << "simulation " << sim_idx <<  " stopped" << std::endl;
                        }
                    }
                }
                else
                    continue;

            }//for each simulators
        });//tbb::parallel
        _ea->update(_environment);

        if(!_eval_queue.empty() && _ea->get_population().get_index(_ea->get_pop_size()-1) != _eval_queue.back() && !_ea->is_finish()){
            int s = 0;
            for(;s < _ea->get_pop_size(); s++)
                if(_ea->get_population().get_index(s) == _eval_queue.back())
                    break;
            for(int i = s+1; i < _ea->get_pop_size(); i++)
                _eval_queue.push_back(_ea->get_population().get_index(i));
            _population_size = _ea->get_pop_size();
        }
    }//if(ea->get_population().size() > 0)
    save_logs();
    if((_eval_queue.empty() && _is_all_simulators_finished()) || _ea->get_population().size() == 0)
    {
        _start_overhead_time = hr_clock::now();
        _ea->epoch();
        save_logs(true);
        _ea->init_next_pop();
        _ea->fill_ind_to_eval(_eval_queue);
        for(int i = 0; i < _eval_queue.size(); i++){
            for(int j = 0; j < _eval_queue.size(); j++){
                assert(!(i != j && _eval_queue[i] == _eval_queue[j]));
            }
        }
        if(verbose)
            std::cout << "-_- GENERATION _-_ " << _ea->get_generation() << " finished" << std::endl;
        _ea->incr_generation();
        _end_overhead_time = hr_clock::now();
        std::stringstream sstr;
        sstr << "overhead," << std::chrono::duration_cast<std::chrono::microseconds>(_start_overhead_time - _reference_time).count()
             << "," << std::chrono::duration_cast<std::chrono::microseconds>(_end_overhead_time - _reference_time).count() << std::endl;
        Logging::saveStringToFile("times.csv",sstr.str());

    }
    if(_ea->is_finish()){
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

void Client::save_logs(bool end_of_gen){
    for(const auto &log : _logging_fcts){
        if(log->isEndOfGen() == end_of_gen && !log->isEndOfRun()){
            log->saveLog(_ea);
        }
    }
}

bool Client::_is_all_simulators_finished(){
    bool yes = true;
    for(int index: _idx_vec)
        yes = yes && index == -1;

    return yes;
}
