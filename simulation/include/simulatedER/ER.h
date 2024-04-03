#ifndef ER_H
#define ER_H

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator> // for reading files and handling the acquired information
#include <fstream> // for storing arrays and using ofstream
#include <list>		// for reading files and handling the acquired information
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <boost/interprocess/managed_shared_memory.hpp>

#include "ARE/EA.h"
#include "ARE/misc/RandNum.h"
#include "ARE/misc/utilities.h"
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Logging.h"
#include "simulatedER/zmq_com.hpp"

namespace are {

namespace sim{

class ER
{
public:

    typedef std::unique_ptr<ER> Ptr;
    typedef std::unique_ptr<const ER> ConstPtr;

    ER(): _context(1)
      , _individual_channel(_context,ZMQ_REP){
        reference_time = hr_clock::now();
    }
    virtual ~ER(){
        if(parameters.get())
            parameters.reset();
        if(properties.get())
            properties.reset();
        if(ea.get())
            ea.reset();
        if(environment.get())
            environment.reset();
        if(currentInd.get())
            currentInd.reset();
        if(randNum.get())
            randNum.reset();
        for(auto& log : logs)
            if(log.get())
                log.reset();
    }


    virtual void initialize();

    void saveSettings();



//    bool loadIndividual(int individualNum);

    ///////////////////////
    /// V-REP Functions ///
    ///////////////////////
    /// Initializes ER as a server to accept genomes from client. If framework is server than just hold information for
    /// one genome. Else, initilizes the first population of individuals.
    virtual void startOfSimulation();

    /// This function is called every simulation step. Note that the behavior of the robot drastically changes when
    /// slowing down the simulation since this function will be called more often. All simulated individuals will be
    /// updated until the maximum simulation time, as specified in the environment class, is reached.
    virtual void handleSimulation();

    /// At the end of the simulation the fitness value of the simulated individual is retrieved and stored in the
    /// appropriate files.
    virtual void endOfSimulation();

    void initIndividual();
    void initEnv(){environment->init();}

    //GETTERS & SETTERS
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const misc::RandNum::Ptr &get_randNum(){return randNum;}
    void set_randNum(const misc::RandNum::Ptr &rn){randNum = rn;}
    void set_startRun(bool b){startRun = b;}
    bool get_startRun(){return startRun;}
    void set_currentIndIndex(size_t num){currentIndIndex = num;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}
    const EA::Ptr &get_ea(){return ea;}
    const Individual::Ptr &get_currentInd(){return currentInd;}
    bool get_evalIsFinish(){return evalIsFinish;}
    int get_nbr_eval(){return nbrEval;}

    zmq::socket_t &get_ind_channel(){return _individual_channel;}

protected:
    ///pointer to settting of EA
    //Settings::Ptr settings;
    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;
    /// pointer to EA
    EA::Ptr ea;
    /// pointer to random number generator of EA
    misc::RandNum::Ptr randNum;
    /// Pointer to the Environment class
    VirtualEnvironment::Ptr environment;
    ///Individual currently evaluated
    Individual::Ptr currentInd;

    ///used to create the environment
    std::function<VirtualEnvironment::Factory> environmentFactory;

    ///used to create the EA Algorithm
    std::function<EA::Factory> EAFactory;

    std::function<Logging::Factory> loggingFactory;

    std::vector<Logging::Ptr> logs;

    void saveLogs(bool endOfGen = true);

    // parameters
    /// Tracks the individual number (corresponding to genomes in the population)
    size_t currentIndIndex = 0;
    /// generation counter
    int generation = 0;
    int nbrEval = 0;
    /// Indicates whether to start the simulation in server mode (received genome)
    bool startRun = true;
    float simulationTime = 0;
    bool client = false;
    int clientID = 0;

    bool evalIsFinish;

    hr_clock::time_point reference_time;
    hr_clock::time_point start_eval_time;
    hr_clock::time_point end_eval_time;

    hr_clock::time_point start_overhead_time;
    hr_clock::time_point end_overhead_time;

    zmq::context_t _context;
    zmq::socket_t _individual_channel;
};

}//sim

}//are

#endif //ER_H
