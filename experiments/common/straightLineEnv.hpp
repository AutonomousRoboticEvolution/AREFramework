#ifndef STRAIGHT_LINE_ENV_HPP
#define STRAIGHT_LINE_ENV_HPP

#include <cmath>
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"

namespace are {

namespace sim{

class StraightLine : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<StraightLine> Ptr;
    typedef std::shared_ptr<const StraightLine> ConstPtr;

    StraightLine(const settings::ParametersMapPtr& params);

    ~StraightLine(){}
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

#endif //STRAIGHT_LINE_ENV_HPP
