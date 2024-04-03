#ifndef LOCOMOTION_PPO_ENV_HPP
#define LOCOMOTION_PPO_ENV_HPP

#include <cmath>
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"
#include "torchrl/envs/AbstractEnv.hpp"
#include "torch/torch.h"

namespace are {

namespace sim{

class LocomotionPPOEnv : public VirtualEnvironment, AbstractEnv
{
public:

    typedef std::shared_ptr<LocomotionPPOEnv> Ptr;
    typedef std::shared_ptr<const LocomotionPPOEnv> ConstPtr;

    LocomotionPPOEnv(const settings::ParametersMapPtr& params);

    ~LocomotionPPOEnv(){}
    void init() override;

    //AbstractEnv Methods
    int64_t GetObservationSize() const override;
    int64_t GetActionSize() const override;
    void ResetImpl() override;
    StepResult StepImpl(const torch::Tensor& action) override;
    torch::Tensor GetObs() const override;

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

#endif //LOCOMOTION_PPO_ENV_HPP
