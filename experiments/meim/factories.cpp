#include "simulatedER/mazeEnv.h"
#include "meim.hpp"
#include "obstacleAvoidance.hpp"
#include "meim_logging.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    return std::make_unique<are::sim::ObstacleAvoidance>(param);
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::MEIM>(rn,st);;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{

    are::TrajectoryLog<are::MEIMIndividual>::Ptr trajlog = std::make_shared<are::TrajectoryLog<are::MEIMIndividual>>();
    logs.push_back(trajlog);

    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::GenomeInfoLog::Ptr gilog = std::make_shared<are::GenomeInfoLog>(fit_log_file);
    logs.push_back(gilog);

    std::string gp_log_file = are::settings::getParameter<are::settings::String>(param,"#parentsPoolFile").value;
    are::ParentsPoolLog::Ptr gplog = std::make_shared<are::ParentsPoolLog>(gp_log_file);
    logs.push_back(gplog);
}
