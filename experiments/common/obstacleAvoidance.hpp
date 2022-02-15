#ifndef OBSTACLE_AVOIDANCE_HPP
#define OBSTACLE_AVOIDANCE_HPP

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

class ObstacleAvoidance : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<ObstacleAvoidance> Ptr;
    typedef std::shared_ptr<const ObstacleAvoidance> ConstPtr;

    ObstacleAvoidance(const settings::ParametersMapPtr& params);

    ~ObstacleAvoidance(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    const std::vector<waypoint> &get_trajectory(){return trajectory;}

    void build_tiled_floor(std::vector<int> &tiles_handles);

    std::pair<int,int> real_coordinate_to_matrix_index(const std::vector<double> &pos);

    const Eigen::MatrixXi &get_visited_zone_matrix(){return grid_zone;}

private:
    int move_counter = 0;
    int number_of_collisions = 0;
    Eigen::MatrixXi grid_zone;
};

} //sim

} //are

#endif //OBSTACLE_AVOIDANCE_HPP

