#include "simulatedER/mazeEnv.h"
#include "BodyPlanTesting.h"
#include "ManLog.h"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    are::Environment::Ptr env = std::make_shared<are::sim::MazeEnv>();
    env->set_parameters(param);
    return env;
}

extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::BODYPLANTESTING>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog = std::make_shared<are::FitnessLog>(fit_log_file);
    logs.push_back(fitlog);

    if(are::settings::getParameter<are::settings::Boolean>(param,"#useQuadric").value){
        std::string quad_file = are::settings::getParameter<are::settings::String>(param,"#quadricFile").value;
        are::sq_genome::QuadricsLog::Ptr quad_log = std::make_shared<are::sq_genome::QuadricsLog>(quad_file);
        logs.push_back(quad_log);

        std::string comp_file = are::settings::getParameter<are::settings::String>(param,"#compGenFile").value;
        are::sq_genome::CompGenLog::Ptr comp_log = std::make_shared<are::sq_genome::CompGenLog>(comp_file);
        logs.push_back(comp_log);

        are::sq_genome::SQGenomeLog::Ptr genlog = std::make_shared<are::sq_genome::SQGenomeLog>();
        logs.push_back(genlog);

        std::string par_file = are::settings::getParameter<are::settings::String>(param,"#parentingFile").value;
        are::sq_genome::ParentingLog::Ptr parlog = std::make_shared<are::sq_genome::ParentingLog>(par_file);
        logs.push_back(parlog);

        // are::sq_cppn::GraphVizLog::Ptr gvlog = std::make_shared<are::sq_genome::GraphVizLog>();
        // logs.push_back(gvlog);

        // std::string ncn_file = are::settings::getParameter<are::settings::String>(param,"#nbrConnNeuFile").value;
        // are::sq_cppn::NbrConnNeurLog::Ptr ncnlog = std::make_shared<are::sq_cppn::NbrConnNeurLog>(ncn_file);
        // logs.push_back(ncnlog);
    }else{
        are::nn2_cppn::NN2CPPNGenomeLog::Ptr genlog = std::make_shared<are::nn2_cppn::NN2CPPNGenomeLog>();
        logs.push_back(genlog);

        std::string par_file = are::settings::getParameter<are::settings::String>(param,"#parentingFile").value;
        are::nn2_cppn::ParentingLog::Ptr parlog = std::make_shared<are::nn2_cppn::ParentingLog>(par_file);
        logs.push_back(parlog);

        are::nn2_cppn::GraphVizLog::Ptr gvlog = std::make_shared<are::nn2_cppn::GraphVizLog>();
        logs.push_back(gvlog);

        std::string ncn_file = are::settings::getParameter<are::settings::String>(param,"#nbrConnNeuFile").value;
        are::nn2_cppn::NbrConnNeurLog::Ptr ncnlog = std::make_shared<are::nn2_cppn::NbrConnNeurLog>(ncn_file);
        logs.push_back(ncnlog);
    }

    std::string tests_log_file = are::settings::getParameter<are::settings::String>(param,"#testsFile").value;
    are::TestsLog::Ptr testslog = std::make_shared<are::TestsLog>(tests_log_file);
    logs.push_back(testslog);

//    if(!are::settings::getParameter<are::settings::Boolean>(param,"#isCPPNGenome").value){
//        are::SkeletonMatrixLog::Ptr skeleton_matrix_log = std::make_shared<are::SkeletonMatrixLog>();
 //       logs.push_back(skeleton_matrix_log);
//    }

    std::string md_cart_wd_log_file = are::settings::getParameter<are::settings::String>(param,"#mdCartWdhFile").value;
    are::morphDescCartWHDLog::Ptr mdcartwdlog = std::make_shared<are::morphDescCartWHDLog>(md_cart_wd_log_file);
    logs.push_back(mdcartwdlog);

    std::string organ_position_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#organPositionDescFile").value;
    are::OrganPositionDescLog::Ptr organ_position_desc_log = std::make_shared<are::OrganPositionDescLog>(organ_position_desc_log_file);
    logs.push_back(organ_position_desc_log);
}
