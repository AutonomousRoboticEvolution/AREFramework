#include "simulatedER/mazeEnv.h"
#include "robustness_test.hpp"
#include "ARE/Logging.h"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::sim::MazeEnv);
    env->set_parameters(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::RobustnessTest(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
    logs.push_back(fitlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::RobustInd>::Ptr bdlog(new are::BehavDescLog<are::RobustInd>(behav_desc_log_file));
    logs.push_back(bdlog);

    are::TrajectoryLog<are::RobustInd>::Ptr trajlog(new are::TrajectoryLog<are::RobustInd>);
    logs.push_back(trajlog);

}
