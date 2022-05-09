#ifndef GRADUAL_ENV_HPP
#define GRADUAL_ENV_HPP

#include <cmath>

#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"
#include "simulatedER/Morphology.h"

namespace are {

namespace sim{

class GradualEnvironment : public VirtualEnvironment
{
public:

    typedef enum fitness_fcts{
        EXPLORATION = 0,
        TARGET,
        FORAGING
    } fitness_fcts;

    typedef struct env_t{
        std::string scene_path;
        std::vector<double> init_position;
        fitness_fcts fitness_fct;
        std::vector<double> target_position;
    } env_t;

    typedef std::shared_ptr<GradualEnvironment> Ptr;
    typedef std::shared_ptr<const GradualEnvironment> ConstPtr;

    GradualEnvironment(const settings::ParametersMapPtr& params);

    ~GradualEnvironment(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const Morphology::Ptr &morph) override;

    ///time point to check the status of the robot
    float timeCheck = 0.0;

    const std::vector<waypoint> &get_trajectory(){return trajectory;}

    void build_tiled_floor(std::vector<int> &tiles_handles);

    std::pair<int,int> real_coordinate_to_matrix_index(const std::vector<double> &pos);

    const Eigen::MatrixXi &get_visited_zone_matrix(){return grid_zone;}

    int get_number_of_scenes(){return environments_info.size();}

    const std::vector<std::vector<waypoint>>& get_trajectories(){return trajectories;}

    void set_current_scene(int cs){current_scene = cs;}

    void load_environments_list(const std::string &file_name, std::vector<env_t> &env_info);

private:
    int move_counter = 0;
    int number_of_collisions = 0;
    Eigen::MatrixXi grid_zone;
    std::vector<env_t> environments_info;
    int current_scene = 0;
    std::vector<double> init_position;
    std::vector<double> target_position;

    std::vector<std::vector<waypoint>> trajectories;

    fitness_fcts _fitness_fcts;

    std::vector<double> _exploration();
    std::vector<double> _targeted_locomotion();
    std::vector<double> _foraging(){return std::vector<double>(0);}

};

} //sim

} //are

#endif //GRADUAL_ENV_HPP

