#ifndef LOCOMOTION_ENV_HPP
#define LOCOMOTION_ENV_HPP

#include <cmath>
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"

namespace are {

namespace sim{

class Locomotion : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<Locomotion> Ptr;
    typedef std::shared_ptr<const Locomotion> ConstPtr;

    Locomotion(const settings::ParametersMapPtr& params);

    ~Locomotion(){}
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

#endif //LOCOMOTION_ENV_HPP
