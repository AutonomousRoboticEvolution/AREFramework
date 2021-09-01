#ifndef PHY_ER_HPP
#define PHY_ER_HPP

#include <chrono>

#include "simLib.h"

#include "ARE/Settings.h"
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Environment.h"
#include "ARE/misc/RandNum.h"
#include "physicalER/pi_individual.hpp"

namespace are {
namespace phy {
namespace generate {

typedef enum robot_state_t{
    READY,
    BUSY,
    FINISHED
} robot_state_t;


class ER
{
public:

    typedef std::unique_ptr<ER> Ptr;
    typedef std::unique_ptr<const ER> ConstPtr;

    ER(){}
    virtual ~ER(){
        if(parameters.get())
            parameters.reset();
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

    void initialize();

    void load_data();
    void write_data();
    void generate();

    void manufacturability_test();

    void save_logs(bool eog=true);

    //GETTERS & SETTERS
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const misc::RandNum::Ptr &get_randNum(){return randNum;}
    void set_randNum(const misc::RandNum::Ptr &rn){randNum = rn;}
    const EA::Ptr &get_ea(){return ea;}
    const Individual::Ptr &get_currentInd(){return currentInd;}

private:
    ///pointer to settting of EA
    //Settings::Ptr settings;
    settings::ParametersMapPtr parameters;
    /// pointer to EA
    EA::Ptr ea;
    /// pointer to random number generator of EA
    misc::RandNum::Ptr randNum;
    /// Pointer to the Environment class
    Environment::Ptr environment;
    ///Individual currently evaluated
    Individual::Ptr currentInd;

    size_t currentIndIndex = 0;
    int nbrEval = 0;

    bool isEnvInit = false;
    bool verbose=false;

    ///used to create the environment
    std::function<Environment::Factory> environmentFactory;

    ///used to create the EA Algorithm
    std::function<EA::Factory> EAFactory;

    std::function<Logging::Factory> loggingFactory;

    std::vector<Logging::Ptr> logs;

    robot_state_t robot_state;

    std::chrono::steady_clock::time_point eval_t1;
    std::chrono::steady_clock::time_point eval_t2;


};

}//generate
}//phy
}//are

#endif //PHY_ER_HPP
