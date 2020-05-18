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

extern "C" are::EA::Ptr EAFactory(const misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
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

    std::string morph_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#morphDescFile").value;
    are::MorphDesLog::Ptr morphdesclog(new are::MorphDesLog(morph_desc_log_file));
    logs.push_back(morphdesclog);

    std::string tests_log_file = are::settings::getParameter<are::settings::String>(param,"#testsFile").value;
    are::TestsLog::Ptr testslog(new are::TestsLog(tests_log_file));
    logs.push_back(testslog);

//    std::string raw_matrix_file = are::settings::getParameter<are::settings::String>(param, "#rawMatrixFile").value;
//    are::RawMatrixLog::Ptr  rawlog(new are::RawMatrixLog(raw_matrix_file));
//    logs.push_back(rawlog);
//
//    std::string protophenotype_file = are::settings::getParameter<are::settings::String>(param, "#protoPhenotypeFile").value;
//    are::ProtoPhenotypeLog::Ptr  protophenotypelog(new are::ProtoPhenotypeLog(protophenotype_file));
//    logs.push_back(protophenotypelog);
}
