#ifndef NIPES_HPP
#define NIPES_HPP


#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib/simLib.h"
#endif

#include "simulatedER/mazeEnv.h"
#include "ARE/learning/ipop_cmaes.hpp"
#include "ARE/learning/Novelty.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "ARE/Settings.h"
<<<<<<< HEAD
#include "simulatedER/obstacleAvoidance.hpp"
#include "simulatedER/exploration.hpp"
#include "simulatedER/multiTargetMaze.hpp"
=======
#include "obstacleAvoidance.hpp"
#include "exploration.hpp"
#include "multiTargetMaze.hpp"
#include "barrelTask.hpp"
#include "push_object.hpp"
#include "env_settings.hpp"
>>>>>>> update_decoding

namespace are{


typedef enum DescriptorType{
    FINAL_POSITION = 0,
    VISITED_ZONES = 1
}DescriptorType;

class NIPESIndividual : public sim::NN2Individual
{
public:
    NIPESIndividual() : sim::NN2Individual(){}
    NIPESIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen)
        : sim::NN2Individual(morph_gen,ctrl_gen){}
    NIPESIndividual(const NIPESIndividual& ind)
        : sim::NN2Individual(ind),
          visited_zones(ind.visited_zones),
          descriptor_type(ind.descriptor_type),
          rewards(ind.rewards){}

    std::string to_string();
    void from_string(const std::string&);
    Eigen::VectorXd descriptor() override;
    void set_visited_zones(const Eigen::MatrixXi& vz){visited_zones = vz;}
    void set_descriptor_type(DescriptorType dt){descriptor_type = dt;}

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
        arch & visited_zones;
        arch & descriptor_type;
        arch & rewards;
      //  arch & energy_cost;
        arch & trajectory;
        arch & object_trajectory;
      //  arch & sim_time;
    }


    int get_number_times_evaluated(){return rewards.size();}
    void reset_rewards(){rewards.clear();}
    void compute_fitness();
    void add_reward(double reward){rewards.push_back(reward);}

    void set_trajectories(const std::vector<std::vector<waypoint>> &trajs){trajectories=trajs;}
    const std::vector<std::vector<waypoint>> &get_trajectories() const {return trajectories;}


    const std::vector<waypoint> &get_object_trajectory() const {return object_trajectory;}
    void set_object_trajectory(const std::vector<waypoint> &obj_traj){object_trajectory = obj_traj;}

private:

    Eigen::MatrixXi visited_zones;
    DescriptorType descriptor_type = FINAL_POSITION;

    std::vector<double> rewards;

    std::vector<std::vector<waypoint>> trajectories;
    std::vector<waypoint> object_trajectory;

};

class NIPES : public EA
{
public:
    struct novelty_params{
        static int k_value;
        static double novelty_thr;
        static double archive_adding_prob;
    };

    NIPES() : EA(){}
    NIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~NIPES(){
        _cma_strat.reset();
    }

    void init() override;
    void epoch() override;
    void init_next_pop() override;
    bool update(const Environment::Ptr&) override;

    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;

    bool is_finish() override;
    bool finish_eval(const Environment::Ptr& env) override;

    bool restarted(){return !_cma_strat->log_stopping_criterias.empty();}
    std::string pop_stopping_criterias(){
        std::string res = _cma_strat->log_stopping_criterias.back();
        _cma_strat->log_stopping_criterias.pop_back();
        return res;
    }
    const std::vector<Eigen::VectorXd> &get_archive(){return archive;}
    void update_pop_info(const std::vector<double>& obj, const Eigen::VectorXd &desc = Eigen::VectorXd::Zero(1));

    const IPOPCMAStrategy::Ptr &get_cma_strat(){return _cma_strat;}
    void set_cma_strat(const IPOPCMAStrategy::Ptr& cmas){_cma_strat = cmas;}

    const std::pair<int,Individual::Ptr> &get_best_individual() const {return best_individual;}

    bool is_healthy_generation(){return healthy_generation;}

protected:
    IPOPCMAStrategy::Ptr _cma_strat;
    cma::CMASolutions best_run;
    bool _is_finish = false;
    std::vector<Eigen::VectorXd> archive;
    int reevaluated = 0;
    std::vector<int> newly_evaluated;
    std::pair<int,Individual::Ptr> best_individual;
    bool healthy_generation = true;

};

}

#endif //NIPES_HPP


