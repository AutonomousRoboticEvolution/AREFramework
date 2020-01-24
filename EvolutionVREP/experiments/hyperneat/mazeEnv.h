#ifndef MAZEENV_H
#define MAZEENV_H

#include <cmath>
#include "ARE/Environment.h"
#include "ARE/Individual.h"

namespace are {

class MazeEnv : public Environment
{
public:
    MazeEnv() : Environment()
    {
        target_position.resize(3);
        final_position.resize(3);
    }
    ~MazeEnv(){}
    void init() override;

    double fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;
private:
    std::vector<double> target_position;
    std::vector<double> final_position;
};

} //are

#endif //MAZEENV_H
