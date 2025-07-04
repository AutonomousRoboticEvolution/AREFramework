#ifndef MAZEENV_H
#define MAZEENV_H

#include <cmath>

#include "simLib/simLib.h"

#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"
#include "simulatedER/Morphology.h"

namespace are {

    namespace sim{

        class MazeEnv : public VirtualEnvironment
        {
        public:

            typedef std::shared_ptr<MazeEnv> Ptr;
            typedef std::shared_ptr<const MazeEnv> ConstPtr;

            MazeEnv();
            MazeEnv(const MazeEnv& env):
                VirtualEnvironment(env),
                target_position(env.target_position),
                move_counter(env.move_counter){}

            ~MazeEnv(){}
            void init() override;

            std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
            float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

            ///time point to check the status of the robot
            float timeCheck = 0.0;


        private:
            std::vector<double> target_position;
            int move_counter = 0;
        };

    } //sim

} //are

#endif //MAZEENV_H
