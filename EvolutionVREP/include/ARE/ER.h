#pragma once
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

#include "ARE/EA.h"
#include "misc/RandNum.h"
#include "misc/utilities.h"
#include "ARE/Environment.h"

namespace are {

class ER
{
public:

    typedef std::unique_ptr<ER> Ptr;
    typedef std::unique_ptr<const ER> ConstPtr;

    ER(){}
    virtual ~ER(){
        parameters.reset();
        properties.reset();
        ea.reset();
        environment.reset();
        currentInd.reset();
        randNum.reset();
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

    //GETTERS & SETTERS
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const misc::RandNum::Ptr &get_randNum(){return randNum;}
    void set_randNum(const misc::RandNum::Ptr &rn){randNum.reset(rn.get());}
    void set_startRun(bool b){startRun = b;}
    bool get_startRun(){return startRun;}
    void set_currentIndIndex(size_t num){currentIndIndex = num;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}


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
    Environment::Ptr environment;
    ///Individual currently evaluated
    Individual::Ptr currentInd;

    ///used to create the environment
    std::function<Environment::Factory> environmentFactory;

    ///used to create the EA Algorithm
    std::function<EA::Factory> EAFactory;

    // parameters
    /// Tracks the individual number (corresponding to genomes in the population)
    size_t currentIndIndex = 0;
    /// generation counter
    int generation = 0;
    /// Indicates whether to start the simulation in server mode (received genome)
    bool startRun = true;
    float simulationTime = 0;
    bool client = false;
};

}//are

#endif //ER_H
