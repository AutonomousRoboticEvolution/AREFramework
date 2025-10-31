#pragma once

#include "simLib/simLib.h"
#include "ARE/misc/eigen_boost_serialization.hpp"
#include "simulatedER/mazeEnv.h"
#include "are_torch/pi_bb_elite.hpp"
#include "ARE/Individual.h"
#include "ARE/Settings.h"
#include "ARE/EA.h"
#include "obstacleAvoidance.hpp"
#include "exploration.hpp"
// #include "multiTargetMaze.hpp"
#include "barrelTask.hpp"
#include "push_object.hpp"
#include "env_settings.hpp"

namespace l = are::learning;

namespace are{

class TensorGenome: public Genome
{
public:
    typedef std::shared_ptr<TensorGenome> Ptr;
    typedef std::shared_ptr<const TensorGenome> ConstPtr;

    TensorGenome() : Genome() {type = "tensor_genome";}
    TensorGenome(misc::RandNum::Ptr rn, settings::ParametersMapPtr param, int id = 0) :
        Genome(rn,param,id){type = "tensor_genome";}
    TensorGenome(const TensorGenome &ngen) :
        Genome(ngen), _tensor(ngen._tensor){}

    Genome::Ptr clone() const override {
        return std::make_shared<TensorGenome>(*this);
    }

    void init() override{}
    void mutate() override{}
    void crossover(const Genome::Ptr &partner, Genome::Ptr child) override{}
    void symmetrical_crossover(const Genome::Ptr &partner, Genome::Ptr child1, Genome::Ptr child2) override{}

    std::string to_string() const override;
    void from_string(const std::string&) override;
    void from_file(const std::string&) override;

    friend class boost::serialization::access;
    template <class archive>
    void save(archive &arch, const unsigned int v) const{
        std::vector<double> params(_tensor.data_ptr<double>(),_tensor.data_ptr<double>() + _tensor.numel());
        arch & boost::serialization::base_object<Genome>(*this);
        arch & params;
        arch & _sections;
    }
    template <class archive>
    void load(archive &arch, const unsigned int v){
        std::vector<double> params;
        arch & boost::serialization::base_object<Genome>(*this);
        arch & params;
        arch & _sections;
        _tensor = torch::from_blob(params.data(),{static_cast<int64_t>(params.size())},torch::TensorOptions().dtype(torch::kDouble)).clone();
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    void set_tensor(const torch::Tensor& tensor){_tensor = tensor.clone();}
    torch::Tensor& tensor(){return _tensor;}
    torch::Tensor get_section(int i);
    void set_sections(const std::vector<int>& sections){_sections = sections;}
    const std::vector<int>& get_sections() const {return _sections;}

private:
    torch::Tensor _tensor;
    std::vector<int> _sections;
};

typedef enum DescriptorType{
    FINAL_POSITION = 0,
    VISITED_ZONES = 1
}DescriptorType;

class PiBBIndividual : public Individual
{
public:
    typedef std::shared_ptr<PiBBIndividual> Ptr;
    typedef std::shared_ptr<const PiBBIndividual> ConstPtr;

    PiBBIndividual() : Individual(){
        visited_zones = Eigen::MatrixXi::Zero(8,8);
    }
    PiBBIndividual(const Genome::Ptr& morph_gen,const TensorGenome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen){
        visited_zones = Eigen::MatrixXi::Zero(8,8);
    }
    PiBBIndividual(const PiBBIndividual& ind)
        : Individual(ind),
          visited_zones(ind.visited_zones),
          descriptor_type(ind.descriptor_type),
          rewards(ind.rewards),
        object_trajectory(ind.object_trajectory){}
    Individual::Ptr clone() override {
        return std::make_shared<PiBBIndividual>(*this);
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
        arch & rollout;
        arch & _immediate_returns;
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

    void set_immediate_returns(const std::vector<double>& returns){_immediate_returns = returns;}
    const std::vector<double>& get_immediate_returns() const {return _immediate_returns;}

    void set_is_current_policy(bool is_current){_is_current_policy = is_current;}
    bool is_current_policy() const {return _is_current_policy;}
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
    std::vector<double> _immediate_returns;
    bool _is_current_policy = false;
};

class PiBB : public EA
{
public:
    typedef std::shared_ptr<PiBB> Ptr;
    typedef std::shared_ptr<const PiBB> ConstPtr;

    PiBB() : EA(){}
    PiBB(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}

    void init() override;
    void epoch() override;
    void init_next_pop() override;
    bool update(const Environment::Ptr&) override;

    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;

    bool is_finish() override;
    bool finish_eval(const Environment::Ptr& env) override;

    void update_pop_info(const std::vector<double>& obj, const Eigen::VectorXd &desc = Eigen::VectorXd::Zero(1));

    const l::PiBBElite::Ptr &get_pibb(){return _pibb;}
    // void set_pibb(const l::PiBB::Ptr& pibb){_pibb = pibb;}

    const PiBBIndividual::Ptr &get_current_policy() const {return _current_policy_ind;}


protected:
    l::PiBBElite::Ptr _pibb;
    bool _is_finish = false;
    int reevaluated = 0;
    std::vector<int> newly_evaluated;
    PiBBIndividual::Ptr _current_policy_ind;
};

}


