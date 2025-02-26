#ifndef PUSH_OBJECT_HPP
#define PUSH_OBJECT_HPP

#include <cmath>
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"

namespace are {

namespace sim{

class PushObject : public VirtualEnvironment
{
public:

    typedef std::shared_ptr<PushObject> Ptr;
    typedef std::shared_ptr<const PushObject> ConstPtr;

    PushObject(const settings::ParametersMapPtr& params);

    ~PushObject(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    void build_tiled_floor(std::vector<int> &tiles_handles);

    const std::vector<waypoint> &get_object_trajectory() const {return object_trajectory;}

private:
    int move_counter = 0;
    std::vector<double> object_initial_position;
    std::vector<double> object_current_position;
    int object_handle = -1;
    std::vector<waypoint> object_trajectory;

};
}

} //are

#endif //PUSH_OBJECT_HPP
