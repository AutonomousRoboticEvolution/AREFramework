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

    std::string tests_log_file = are::settings::getParameter<are::settings::String>(param,"#testsFile").value;
    are::TestsLog::Ptr testslog(new are::TestsLog(tests_log_file));
    logs.push_back(testslog);

//    std::string raw_matrix_file = are::settings::getParameter<are::settings::String>(param, "#rawMatrixFile").value;
//    are::RawMatrixLog::Ptr  rawlog(new are::RawMatrixLog(raw_matrix_file));
//    logs.push_back(rawlog);
//

//    std::string graphmatrix_file = are::settings::getParameter<are::settings::String>(param, "#graphMatrixFile").value;
//    are::GraphMatrixLog::Ptr  graphmatrixlog(new are::GraphMatrixLog(graphmatrix_file));
//    logs.push_back(graphmatrixlog);

    std::string md_cart_wd_log_file = are::settings::getParameter<are::settings::String>(param,"#mdCartWdhFile").value;
    are::morphDescCartWHDLog::Ptr mdcartwdlog(new are::morphDescCartWHDLog(md_cart_wd_log_file));
    logs.push_back(mdcartwdlog);

//    std::string md_matrix_proto_log_file = are::settings::getParameter<are::settings::String>(param,"#mdMatrixProtoFile").value;
//    are::ProtoMatrixLog::Ptr mdmatrixprotolog(new are::ProtoMatrixLog(md_matrix_proto_log_file));
//    logs.push_back(mdmatrixprotolog);

//    std::string md_matrix_bincppn_log_file = are::settings::getParameter<are::settings::String>(param,"#mdMatrixBinCPPNFile").value;
//    are::BinCPPNMatrixLog::Ptr mdmatrixbincppnlog(new are::BinCPPNMatrixLog(md_matrix_bincppn_log_file));
//    logs.push_back(mdmatrixbincppnlog);
//
//    std::string md_matrix_inter_log_file = are::settings::getParameter<are::settings::String>(param,"#mdMatrixInterFile").value;
//    are::InterMatrixLog::Ptr mdmatrixinterlog(new are::InterMatrixLog(md_matrix_inter_log_file));
//    logs.push_back(mdmatrixinterlog);
}
