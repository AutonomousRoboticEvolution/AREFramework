#ifndef SIM_LOGGING_HPP
#define SIM_LOGGING_HPP

#include "simulatedER/mazeEnv.h"
#include "ARE/Logging.h"

namespace are {

namespace sim{

template <class ind_t>
class TrajectoryLog : public Logging
{
public:
    TrajectoryLog() : Logging(true){}
    void saveLog(EA::Ptr& ea){
        std::ofstream ofs;
        int generation = ea->get_generation();
        for(size_t i = 0; i < ea->get_population().size(); i++){
            std::vector<are::sim::waypoint> traj = std::dynamic_pointer_cast<ind_t>(ea->getIndividual(i))->get_trajectory();
            std::stringstream filepath;
            filepath << "/traj_" << generation << "_" << i;
            logFile = filepath.str();

            if(!openOLogFile(ofs))
                return;
            for(const are::sim::waypoint& wp: traj)
                ofs << wp.to_string() << std::endl;
            ofs.close();
        }
    }
    void loadLog(const std::string &file){}
};

}//sim

}//are

#endif //SIM_LOGGING_HPP
