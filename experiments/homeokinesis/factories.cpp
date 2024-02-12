#include "simulatedER/mazeEnv.h"
#include "homeokinesis.hpp"
#include "obstacleAvoidance.hpp"


extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    return std::make_unique<are::sim::ObstacleAvoidance>(param);
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::Homeokinesis>(rn,st);;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{

    are::TrajectoryLog<are::HomeoInd>::Ptr trajlog = std::make_shared<are::TrajectoryLog<are::HomeoInd>>();
    logs.push_back(trajlog);
}
