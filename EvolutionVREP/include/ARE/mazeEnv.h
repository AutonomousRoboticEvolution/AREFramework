#ifndef MAZEENV_H
#define MAZEENV_H

#include <cmath>
#include "ARE/Environment.h"
#include "ARE/Individual.h"

namespace are {

struct waypoint{
    float position[3];
    float orientation[3];

    std::string to_string() const{
        std::stringstream sstr;
        sstr << position[0] << "," << position[1] << "," << position[2] << ";"
             << orientation[0] << "," << orientation[1] << "," << orientation[2];
        return sstr.str();
    }

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & position;
        arch & orientation;
    }

};

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
    const std::vector<double> &get_target_position(){return target_position;}
    const std::vector<waypoint> &get_trajectory(){return trajectory;}

    void build_tiled_floor(std::vector<int> &tiles_handles);

private:
    std::vector<double> start_position;
    std::vector<double> target_position;
    std::vector<double> final_position;
    std::vector<waypoint> trajectory;
    int move_counter = 0;
};

} //are

#endif //MAZEENV_H
