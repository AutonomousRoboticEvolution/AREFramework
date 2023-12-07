#ifndef VIRTUAL_ENVIRONMENT_HPP
#define VIRTUAL_ENVIRONMENT_HPP

#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib/simLib.h"
#endif

#include "ARE/Environment.h"
#include "simulatedER/Morphology.h"

namespace are{

namespace sim {

class VirtualEnvironment : public Environment
{
public:

    typedef std::shared_ptr<VirtualEnvironment> Ptr;
    typedef std::shared_ptr<const VirtualEnvironment> ConstPtr;
    typedef VirtualEnvironment::Ptr (Factory)(const settings::ParametersMapPtr&);


    VirtualEnvironment(){}
    VirtualEnvironment(const VirtualEnvironment& env) :
        envObjectHandles(env.envObjectHandles),
        maxTime(env.maxTime){}
    ~VirtualEnvironment(){}

    /**
     * @brief Initialize the default environment scene and simulation time step size
     */
    virtual void init();

    void sceneLoader();

    /**
     * @brief Calculate the fitness value of the robot
     * @param morph The pointer of the robot (morphology)
     */
    virtual std::vector<double> fitnessFunction(const Individual::Ptr &ind) = 0;

    virtual void update_info(double time) override{}

    /**
     * @brief update the info of the objects (e.g. robot) in the environments
     * @param morph The pointer of the robot (morphology)
     */
    virtual float updateEnv(float simulationTime, const Morphology::Ptr &morph) = 0;


    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const std::vector<int> &get_envObjectHandles(){return envObjectHandles;}
    float get_maxTime(){return maxTime;}
    const std::string &get_name(){return name;}
    void set_randNum(misc::RandNum::Ptr &rn){randNum = rn;}

    void build_tiled_floor(std::vector<int> &tiles_handles);

    //wrapper for coppeliaSim function
    double get_sim_time(){return simGetSimulationTime();}
    std::vector<double> get_object_position(int handle);

protected:
    ///handle of the object in the environment
    std::vector<int> envObjectHandles;
    ///maximum simulation time
    float maxTime = 0.0;

};

} //sim

} //are

#endif //VIRTUAL_ENVIRONMENT_HPP
