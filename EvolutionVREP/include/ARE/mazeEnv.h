#ifndef MAZEENV_H
#define MAZEENV_H

#include <cmath>
#include "ARE/Environment.h"
#include "ARE/Individual.h"

namespace are {


class MazeEnv : public Environment
{
public:

    typedef std::shared_ptr<MazeEnv> Ptr;
    typedef std::shared_ptr<const MazeEnv> ConstPtr;

    MazeEnv();

    ~MazeEnv(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    const std::vector<double> &get_final_position(){return final_position;}

    void build_tiled_floor(std::vector<int> &tiles_handles);

private:
    std::vector<double> target_position;
    int move_counter = 0;

    void build_tiled_floor(std::vector<int> &tiles_handles);
};

} //are

#endif //MAZEENV_H
