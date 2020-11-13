#include "testEnv.h"
#include "LC_NSMS.h"
#include "ManLog.h"

extern "C" are::Environment::Ptr environmentFactory
        (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env(new are::TestEnv);
    env->set_parameters(param);
    return env;
}

extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    are::EA::Ptr ea;

    ea.reset(new are::LC_NSMS(st));

    ea->set_randomNum(rn);
    return ea;
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog(new are::FitnessLog(fit_log_file));
    logs.push_back(fitlog);

    std::string genome_log_file = are::settings::getParameter<are::settings::String>(param,"#genomeFile").value;
    are::GenomeLog::Ptr genlog(new are::GenomeLog(genome_log_file));
    logs.push_back(genlog);

    std::string tests_log_file = are::settings::getParameter<are::settings::String>(param,"#testsFile").value;
    are::TestsLog::Ptr testslog(new are::TestsLog(tests_log_file));
    logs.push_back(testslog);

    std::string md_cart_wd_log_file = are::settings::getParameter<are::settings::String>(param,"#mdCartWdhFile").value;
    are::morphDescCartWHDLog::Ptr mdcartwdlog(new are::morphDescCartWHDLog(md_cart_wd_log_file));
    logs.push_back(mdcartwdlog);

    std::string md_sym_log_file = are::settings::getParameter<are::settings::String>(param,"#mdSymFile").value;
    are::morphDescSymLog::Ptr mdsymlog(new are::morphDescSymLog(md_sym_log_file));
    logs.push_back(mdsymlog);

    std::string md_matrix_proto_log_file = are::settings::getParameter<are::settings::String>(param,"#mdMatrixProtoFile").value;
    are::ProtoMatrixLog::Ptr mdmatrixprotolog(new are::ProtoMatrixLog(md_matrix_proto_log_file));
    logs.push_back(mdmatrixprotolog);
}
