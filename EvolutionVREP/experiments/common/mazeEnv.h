#ifndef MAZEENV_H
#define MAZEENV_H

#include <cmath>
#include "ARE/Environment.h"
#include "ARE/Individual.h"

namespace are {

class MazeEnv : public Environment
{
public:
    MazeEnv() : Environment(), randNum(0)
    {
        target_position.resize(3);
        final_position.resize(3);
        name = "mazeEnv";
    }
    ~MazeEnv(){}
    void init() override;

    double fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    void init_randNum(int seed){randNum.setSeed(seed);}

    const std::vector<double> &get_final_position(){return final_position;}

private:
    std::vector<double> target_position;
    std::vector<double> final_position;
    misc::RandNum randNum;
};

} //are

#endif //MAZEENV_H
