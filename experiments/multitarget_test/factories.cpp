#include "multitarget_test.hpp"
#include "multiTargetMaze.hpp"


extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::sim::MultiTargetMaze::Ptr env = std::make_shared<are::sim::MultiTargetMaze>(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::MultiTargetTest>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{

    are::MultiTrajectoriesLog::Ptr trajlog = std::make_shared<are::MultiTrajectoriesLog>();
    logs.push_back(trajlog);

    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#rewardFile").value;
    are::RewardsLog::Ptr fitlog = std::make_shared<are::RewardsLog>(fit_log_file);
    logs.push_back(fitlog);
}
