#include "simulatedER/mazeEnv.h"
#include "M_NIPES.hpp"
#include "MNIPESLoggings.hpp"
#include "env_settings.hpp"
#include "obstacleAvoidance.hpp"
#include "barrelTask.hpp"
#include "exploration.hpp"
#include "locomotion.hpp"
#include "hill_climbing.hpp"
#include "push_object.hpp"

extern "C" are::Environment::Ptr environmentFactory
    (const are::settings::ParametersMapPtr& param)
{
    int env_type = are::settings::getParameter<are::settings::Integer>(param,"#envType").value;
    are::Environment::Ptr env;
    if(env_type == are::sim::MAZE){
        env = std::make_shared<are::sim::MazeEnv>();
        env->set_parameters(param);
    }
    else if(env_type == are::sim::OBSTACLES)
        env = std::make_shared<are::sim::ObstacleAvoidance>(param);
    else if(env_type == are::sim::MULTI_TARGETS)
        env = std::make_shared<are::sim::MultiTargetMaze>(param);
    else if(env_type == are::sim::LOCOMOTION)
        env = std::make_shared<are::sim::Locomotion>(param);
    else if(env_type == are::sim::BARREL)
        env = std::make_shared<are::sim::BarrelTask>(param);
    else if(env_type == are::sim::HILL_CLIMBING)
        env = std::make_shared<are::sim::HillClimbing>(param);
    else if(env_type == are::sim::PUSH_OBJECT)
        env = std::make_shared<are::sim::PushObject>(param);
    else
        std::cerr << "factory env: unknown environement" << std::endl;

    return env;
}



extern "C" are::EA::Ptr EAFactory(const are::misc::RandNum::Ptr &rn, const are::settings::ParametersMapPtr &st)
{
    return std::make_unique<are::M_NIPES>(rn,st);
}

extern "C" void loggingFactory(std::vector<are::Logging::Ptr>& logs,
                               const are::settings::ParametersMapPtr &param)
{
    std::string fit_log_file = are::settings::getParameter<are::settings::String>(param,"#fitnessFile").value;
    are::GenomeInfoLog::Ptr gilog = std::make_shared<are::GenomeInfoLog>(fit_log_file);
    logs.push_back(gilog);

    std::string gp_log_file = are::settings::getParameter<are::settings::String>(param,"#genomesPoolFile").value;
    are::GenomesPoolLog::Ptr gplog = std::make_shared<are::GenomesPoolLog>(gp_log_file);
    logs.push_back(gplog);

    std::string bgi_log_file = are::settings::getParameter<are::settings::String>(param,"#bestGenomesInfoFile").value;
    are::BestGenomesArchiveLog::Ptr bgilog = std::make_shared<are::BestGenomesArchiveLog>(bgi_log_file);
    logs.push_back(bgilog);

    are::ControllersLog::Ptr ctrlslog = std::make_shared<are::ControllersLog>();
    logs.push_back(ctrlslog);


    bool use_ctrl_arch = are::settings::getParameter<are::settings::Boolean>(param,"#useControllerArchive").value;
    if(use_ctrl_arch){
        are::ControllerArchiveLog::Ptr calog = std::make_shared<are::ControllerArchiveLog>();
        logs.push_back(calog);
    }

}


