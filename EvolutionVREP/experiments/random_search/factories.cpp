#include "ARE/mazeEnv.h"
#include "ARE/NNParamGenome.hpp"
#include "random_search.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::MazeEnv);
    env->set_parameters(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::RandomSearch(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
    logs.push_back(fitlog);

    std::string eval_time_log_file = are::settings::getParameter<are::settings::String>(param,"#evalTimeFile").value;
    are::EvalTimeLog::Ptr etlog(new are::EvalTimeLog(eval_time_log_file));
    std::dynamic_pointer_cast<are::EvalTimeLog>(etlog)
            ->set_end_of_gen(are::settings::getParameter<are::settings::Integer>(param,"#instanceType").value == 1);
    logs.push_back(etlog);

    are::NNParamGenomeLog::Ptr nnpglog(new are::NNParamGenomeLog);
    logs.push_back(nnpglog);

    are::TrajectoryLog<are::NN2Individual>::Ptr trajlog(new are::TrajectoryLog<are::NN2Individual>);
    logs.push_back(trajlog);
}
