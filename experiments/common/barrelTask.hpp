#ifndef BARRELTASK_HPP
#define BARRELTASK_HPP

#include <cmath>

#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"
#include "simulatedER/Morphology.h"

namespace are {

namespace sim{

class BarrelTask : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<BarrelTask> Ptr;
    typedef std::shared_ptr<const BarrelTask> ConstPtr;

    BarrelTask(const settings::ParametersMapPtr& params);

    ~BarrelTask(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    const std::vector<waypoint> &get_trajectory(){return trajectory;}

    void build_tiled_floor(std::vector<int> &tiles_handles);

    int get_number_of_targets(){return barrel_initial_positions.size();}
    const std::vector<double> &get_current_target(){return barrel_initial_positions[current_target];}

    const std::vector<std::vector<waypoint>>& get_trajectories(){return trajectories;}

private:
    int current_target = 0;
    int move_counter = 0;

    std::vector<std::vector<double>> barrel_initial_positions;
    std::vector<double> barrel_current_position;
    std::vector<double> target_position;
    int barrel_handle;
    std::vector<std::vector<waypoint>> trajectories;

};

} //sim

} //are

#endif //BARRELTASK_HPP

