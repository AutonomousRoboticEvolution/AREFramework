#ifndef STRAIGHT_LOCOMOTION_ENV_HPP
#define STRAIGHT_LOCOMOTION_ENV_HPP

#include <cmath>
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"

namespace are {

namespace sim{

class StraightLocomotion : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<StraightLocomotion> Ptr;
    typedef std::shared_ptr<const StraightLocomotion> ConstPtr;

    StraightLocomotion(const settings::ParametersMapPtr& params);

    ~StraightLocomotion(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    void build_tiled_floor(std::vector<int> &tiles_handles);

private:
    int move_counter = 0;
    /// Straight line variables
};
}

} //are

#endif //STRAIGHT_LOCOMOTION_ENV_HPP
