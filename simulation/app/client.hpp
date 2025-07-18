#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include <memory>

#include "ARE/misc/RandNum.h"
#include "ARE/Settings.h"
#include "ARE/EA.h"
#include "ARE/Logging.h"

#include "simulator.hpp"
#include "simulatedER/VirtualEnvironment.hpp"


namespace are{
namespace sim{

enum EvalOrder{
    FIFO = 0,
    FILO,
    RANDOM,
    FIFO_RANDOM
};

class Client
{
public:
    typedef std::unique_ptr<Client> Ptr;
    typedef std::unique_ptr<const Client> ConstPtr;

    Client(){}

    bool init(int nbr_instances,int port = 10000);
    void initialize();
    void execute();
    bool update_simulators();
    void individuals_distribution();

    const settings::ParametersMapPtr &get_parameters(){return _parameters;}
    void set_parameters(const settings::ParametersMapPtr &param){_parameters = param;}
    const misc::RandNum::Ptr &get_rand_num(){return _rand_num;}
    void set_rand_num(const misc::RandNum::Ptr &rn){_rand_num = rn;}

    void save_logs(bool end_of_gen = false);

private:
    ///pointer to settting of EA
    settings::ParametersMapPtr _parameters;
    /// pointer to EA
    EA::Ptr _ea;
    /// pointer to random number generator of EA
    misc::RandNum::Ptr _rand_num;
    /// Pointer to the Environment class
    VirtualEnvironment::Ptr _environment;
    ///Individual currently evaluated
    Individual::Ptr _current_ind;

    ///factory to create the environment
    std::function<VirtualEnvironment::Factory> _environment_factory;
    ///factory to create the EA Algorithm
    std::function<EA::Factory> _ea_factory;
    ///factory to create the logging fuctions
    std::function<Logging::Factory> _logging_factory;

    std::vector<Logging::Ptr> _logging_fcts;
    int _population_size;

    std::vector<Simulator> _simulators;
    int _nbr_of_instances = 1;
    std::vector<Individual::Ptr> _ind_vec;
    std::vector<int> _idx_vec;
    std::vector<int> _eval_queue;


    hr_clock::time_point _reference_time;
    hr_clock::time_point _start_eval_time;
    hr_clock::time_point _end_eval_time;

    hr_clock::time_point _start_overhead_time;
    hr_clock::time_point _end_overhead_time;

    bool _is_all_simulators_finished();
    const int _max_connection_trials = 3;
};

}//are
}//sim



#endif //CLIENT_HPP
