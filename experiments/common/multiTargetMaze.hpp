#ifndef MULTITARGETMAZE_HPP
#define MULTITARGETMAZE_HPP

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

class MultiTargetMaze : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<MultiTargetMaze> Ptr;
    typedef std::shared_ptr<const MultiTargetMaze> ConstPtr;

    MultiTargetMaze(const settings::ParametersMapPtr& params);

    ~MultiTargetMaze(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    const std::vector<waypoint> &get_trajectory(){return trajectory;}

    void build_tiled_floor(std::vector<int> &tiles_handles);

    int get_number_of_targets(){return target_positions.size();}
    const std::vector<double> &get_current_target(){return target_positions[current_target];}

    const std::vector<std::vector<waypoint>>& get_trajectories(){return trajectories;}

private:
    int current_target = 0;
    std::vector<std::vector<double>> target_positions;
    int move_counter = 0;
    std::vector<std::vector<waypoint>> trajectories;

};

} //sim

} //are

#endif //MULTITARGETMAZE_HPP

