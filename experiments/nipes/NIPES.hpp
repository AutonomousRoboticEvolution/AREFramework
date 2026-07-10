#ifndef NIPES_HPP
#define NIPES_HPP



#include "simLib/simLib.h"

#include "simulatedER/mazeEnv.h"
#include "ARE/learning/ipop_cmaes.hpp"
#include "ARE/learning/Novelty.hpp"
#include "ARE/Individual.h"
#include "ARE/Settings.h"
#include "ARE/EA.h"
#include "obstacleAvoidance.hpp"
#include "exploration.hpp"
// #include "multiTargetMaze.hpp"
#include "barrelTask.hpp"
#include "push_object.hpp"
#include "env_settings.hpp"

namespace are{


typedef enum DescriptorType{
    FINAL_POSITION = 0,
    VISITED_ZONES = 1
}DescriptorType;

class NIPESIndividual : public Individual
{
public:
    NIPESIndividual() : Individual(){
        visited_zones = Eigen::MatrixXi::Zero(8,8);
    }
    NIPESIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen){
        visited_zones = Eigen::MatrixXi::Zero(8,8);
    }
    NIPESIndividual(const NIPESIndividual& ind)
        : Individual(ind),
          visited_zones(ind.visited_zones),
          descriptor_type(ind.descriptor_type),
          rewards(ind.rewards),
        object_trajectory(ind.object_trajectory){}
    Individual::Ptr clone() override {
        return std::make_shared<NIPESIndividual>(*this);
    }
    std::string to_string() const override;
    void from_string(const std::string&) override;
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
        // arch & rewards;
      //  arch & energy_cost;
        arch & trajectory;
        arch & trajectories;
        arch & object_trajectory;
        //arch & rollout;
      //  arch & sim_time;
    }

    void addObjective(double obj){objectives.push_back(obj);}

    int get_number_times_evaluated(){return rewards.size();}
    void reset_rewards(){rewards.clear();}
    void compute_fitness();
    void add_reward(double reward){rewards.push_back(reward);}

    void set_trajectories(const std::vector<std::vector<waypoint>> &trajs){trajectories=trajs;}
    const std::vector<std::vector<waypoint>> &get_trajectories() const {return trajectories;}


    const std::vector<waypoint> &get_object_trajectory() const {return object_trajectory;}
    void set_object_trajectory(const std::vector<waypoint> &obj_traj){object_trajectory = obj_traj;}

    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}
    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}

private:

    void createController() override;
    void createMorphology() override;

    Eigen::MatrixXi visited_zones;
    DescriptorType descriptor_type = FINAL_POSITION;

    std::vector<double> rewards;
    std::vector<double> final_position;
    std::vector<waypoint> trajectory;
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


