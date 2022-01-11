#include "image_processing/realMaze.hpp"
#include "random_controller.hpp"
#include "ARE/Logging.h"
#include "ARE/NNParamGenome.hpp"



extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::RealMaze);
    env->set_parameters(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::RandomController(rn,st));
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{

//    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
//    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
//    logs.push_back(fitlog);


    are::TrajectoryLog<are::phy::NN2Individual>::Ptr trajlog(new are::TrajectoryLog<are::phy::NN2Individual>);
    logs.push_back(trajlog);

    are::NNParamGenomeLog::Ptr ctrl_gen_log(new are::NNParamGenomeLog);
    logs.push_back(ctrl_gen_log);

}

