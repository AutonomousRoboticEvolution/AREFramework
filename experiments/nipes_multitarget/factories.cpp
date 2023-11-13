#include "multiTargetMaze.hpp"
#include "NIPES.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "NIPESLoggings.hpp"
#include "simulatedER/Logging.hpp"
#include "barrelTask.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
    if(env_type == 0){
        are::sim::VirtualEnvironment::Ptr env = std::make_shared<are::sim::MultiTargetMaze>(param);
        return env;
    }
    if(env_type == 1){
        are::sim::VirtualEnvironment::Ptr env = std::make_shared<are::sim::BarrelTask>(param);
        return env;
    }
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::NIPES>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::FitnessLog::Ptr fitlog = std::make_shared<are::FitnessLog>(fit_log_file);
    logs.push_back(fitlog);

    std::string behav_desc_log_file = are::settings::getParameter<are::settings::String>(param,"#behavDescFile").value;
    are::BehavDescLog<are::sim::NN2Individual>::Ptr bdlog = std::make_shared<are::BehavDescLog<are::sim::NN2Individual>>(behav_desc_log_file);
    logs.push_back(bdlog);

    are::NNParamGenomeLog::Ptr nnpglog = std::make_shared<are::NNParamGenomeLog>();
    logs.push_back(nnpglog);

    are::TrajectoryLog<are::sim::NN2Individual>::Ptr trajlog = std::make_shared<are::TrajectoryLog<are::sim::NN2Individual>>();
    logs.push_back(trajlog);

    std::string stop_crit_log_file = are::settings::getParameter<are::settings::String>(param,"#stopCritFile").value;
    are::StopCritLog::Ptr sclog = std::make_shared<are::StopCritLog>(stop_crit_log_file);
    logs.push_back(sclog);

    std::string archive_log_file = are::settings::getParameter<are::settings::String>(param,"#archiveFile").value;
    are::ArchiveLog<are::NIPES>::Ptr arclog = std::make_shared<are::ArchiveLog<are::NIPES>>(archive_log_file);
    logs.push_back(arclog);

    std::string ec_log_file = are::settings::getParameter<are::settings::String>(param,"#energyCostFile").value;
    are::EnergyCostLog<are::sim::NN2Individual>::Ptr eclog = std::make_shared<are::EnergyCostLog<are::sim::NN2Individual>>(ec_log_file);
    logs.push_back(eclog);

    std::string st_log_file = are::settings::getParameter<are::settings::String>(param,"#simTimeFile").value;
    are::SimTimeLog<are::sim::NN2Individual>::Ptr stlog = std::make_shared<are::SimTimeLog<are::sim::NN2Individual>>(st_log_file);
    logs.push_back(stlog);

}

