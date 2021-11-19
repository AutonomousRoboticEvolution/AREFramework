#ifndef MAZEENV_H
#define MAZEENV_H

#include <cmath>


#include "v_repLib.h"


#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"
#include "simulatedER/Morphology.h"

namespace are {

    namespace sim{

        class multiTarget : public VirtualEnvironment
        {
        public:

            typedef std::shared_ptr<multiTarget> Ptr;
            typedef std::shared_ptr<const multiTarget> ConstPtr;

            multiTarget();

            ~multiTarget(){}
            void init() override;

            std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
            float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

            ///time point to check the status of the robot
            float timeCheck = 0.0;

            const std::vector<waypoint> &get_trajectory(){return trajectory;}
            void load_target_positions();
            void build_tiled_floor(std::vector<int> &tiles_handles);

        private:
            int current_target = 0;
            std::vector<double> target_position;
            std::vector<std::vector<double>> target_position_all;
            int move_counter = 0;
        };

    } //sim

} //are

#endif //MAZEENV_H
