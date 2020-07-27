#include "ARE/mazeEnv.h"
#include "ARE/Logging.h"
#include "ARE/learning/Novelty.hpp"
#include "MB_NIPES.hpp"
#include "MB_NIPESLoggings.hpp"

extern "C" are::Environment::Ptr environmentFactory(const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::MazeEnv);
    env->set_parameters(param);
    return env;
}



extern "C" are::EA::Ptr EAFactory(const misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::MB_NIPES(st));

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

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::NN2Individual>::Ptr bdlog(new are::BehavDescLog<are::NN2Individual>(behav_desc_log_file));
    logs.push_back(bdlog);

    are::NNParamGenomeLog::Ptr nnpglog(new are::NNParamGenomeLog);
    logs.push_back(nnpglog);

    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
    are::ArchiveLog<are::MB_NIPES>::Ptr arclog(new are::ArchiveLog<are::MB_NIPES>(archive_log_file));
    logs.push_back(arclog);

//    std::string bo_log_file = are::settings::getParameter<are::settings::String>(param,"#BOSamplesFile").value;
//    are::BOLog::Ptr bolog(new are::BOLog(bo_log_file));
//    logs.push_back(bolog);

    std::string learner_log_file = are::settings::getParameter<are::settings::String>(param,"#LearnerSerialFile").value;
    are::LearnerSerialLog::Ptr lslog(new are::LearnerSerialLog(learner_log_file));
    logs.push_back(lslog);
}
