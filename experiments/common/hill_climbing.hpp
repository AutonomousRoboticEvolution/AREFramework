#ifndef HILL_CLIMBING_HPP
#define HILL_CLIMBING_HPP

#include <cmath>
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"

namespace are {

namespace sim{

class HillClimbing : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<HillClimbing> Ptr;
    typedef std::shared_ptr<const HillClimbing> ConstPtr;

    HillClimbing(const settings::ParametersMapPtr& params);

    ~HillClimbing(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    void build_tiled_floor(std::vector<int> &tiles_handles);

private:
    int move_counter = 0;
    double best_height = 0;
    /// Straight line variables
};
}

} //are

#endif //HILL_CLIMBING_HPP
