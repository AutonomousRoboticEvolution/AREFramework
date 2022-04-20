#ifndef M_NIPES_HPP
#define M_NIPES_HPP

#include <functional>

#include "ARE/learning/controller_archive.hpp"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/nn2/NN2Control.hpp"
#include "cmaes_learner.hpp"
#include "ARE/misc/eigen_boost_serialization.hpp"
#include "obstacleAvoidance.hpp"
#include "multiTargetMaze.hpp"
#include "barrelTask.hpp"
#include "exploration.hpp"

//TO DO find a way to flush the population

namespace are {
using CPPNMorph = sim::Morphology_CPPNMatrix;

typedef enum task_t{
    MAZE = 0,
    OBSTACLES = 1,
    MULTI_TARGETS = 2,
    EXPLORATION = 3,
    BARREL = 4
} task_t;

typedef struct learner_t{
    learner_t(){}
    learner_t(const learner_t& l) :
        morph_genome(l.morph_genome),
        ctrl_learner(l.ctrl_learner){}
    learner_t(const NN2CPPNGenome &mg): morph_genome(mg){}
    NN2CPPNGenome morph_genome;
    CMAESLearner ctrl_learner;
} learner_t;

typedef struct genome_t{
    genome_t(){}
    genome_t(const genome_t& g) :
        morph_genome(g.morph_genome),
        ctrl_genome(g.ctrl_genome),
        objectives(g.objectives),
        age(g.age),
        trajectories(g.trajectories),
        behavioral_descriptor(g.behavioral_descriptor){}
    genome_t(const NN2CPPNGenome &mg, const NNParamGenome &cg, const std::vector<double> &obj) :
        morph_genome(mg), ctrl_genome(cg), objectives(obj), age(0){}
    NN2CPPNGenome morph_genome;
    NNParamGenome ctrl_genome;
    std::vector<double> objectives;
    std::vector<double> rewards;
    int age;
    std::vector<std::vector<waypoint>> trajectories;
    Eigen::VectorXd behavioral_descriptor;
}genome_t;

typedef std::function<NN2CPPNGenome(const std::vector<genome_t>&)> selection_fct_t;

typedef struct SelectionFunctions{
    static selection_fct_t best_of_subset;
}SelectionFunctions;

typedef std::function<double(const CMAESLearner&)> fitness_fct_t;

typedef struct FitnessFunctions{
    static fitness_fct_t best_fitness;
    static fitness_fct_t average_fitness;
    static fitness_fct_t learning_progress;
}FitnessFunctions;

typedef enum FitnessType{
    BEST_FIT = 0,
    AVG_FIT = 1,
    LEARNING_PROG = 2,
    DIVERSITY = 3
}FitnessType;

typedef enum DescriptorType{
    FINAL_POSITION = 0,
    VISITED_ZONES = 1
}DescriptorType;

class M_NIPESIndividual : public Individual
{
public:
    M_NIPESIndividual() :  Individual(){}
    M_NIPESIndividual(const NN2CPPNGenome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {}
    M_NIPESIndividual(const M_NIPESIndividual& ind) :
        Individual(ind),
        testRes(ind.testRes),
        graphMatrix(ind.graphMatrix),
        morphDesc(ind.morphDesc),
        symDesc(ind.symDesc),
        final_position(ind.final_position),
        trajectories(ind.trajectories),
        energy_cost(ind.energy_cost),
        sim_time(ind.sim_time),
        controller_archive(ind.controller_archive),
        nbr_dropped_eval(ind.nbr_dropped_eval),
        descriptor_type(ind.descriptor_type)
    {}

    Individual::Ptr clone() override {
        return std::make_shared<M_NIPESIndividual>(*this);
    }

    void update(double delta_time) override;

    //specific to the current ARE arenas
    Eigen::VectorXd descriptor();
    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}
    void set_trajectories(const std::vector<std::vector<waypoint>>& traj){trajectories = traj;}
    const std::vector<std::vector<waypoint>>& get_trajectories(){return trajectories;}
    double get_energy_cost(){return energy_cost;}
    double get_sim_time(){return sim_time;}
    void set_ctrl_genome(const NNParamGenome::Ptr &gen){std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome) = gen;}

    void setManRes();

    /// Setters for descritors
    void setMorphDesc();


    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & learner;
        arch & morphGenome;
        arch & ctrlGenome;
        arch & final_position;
//        arch & energy_cost;
        arch & trajectories;
//        arch & sim_time;
        arch & controller_archive;
        arch & morphDesc;
        arch & nbr_dropped_eval;
        arch & descriptor_type;
        arch & copy_rewards;
    }

    std::string to_string() override;
    void from_string(const std::string &) override;


    Eigen::VectorXd getMorphDesc(){return  morphDesc;}

    void set_ctrl_archive(const ControllerArchive& ctrl_arc){controller_archive = ctrl_arc;}

    bool is_actuated(){return !no_actuation;}
    bool has_sensor(){return !no_sensors;}

    void set_nbr_dropped_eval(int nde){nbr_dropped_eval = nde;}
    void incr_nbr_dropped_eval(){nbr_dropped_eval++;}
    int get_nbr_dropped_eval(){return nbr_dropped_eval;}
    void set_descriptor_type(DescriptorType dt){descriptor_type = dt;}
    void set_visited_zones(const Eigen::MatrixXi& vz){visited_zones = vz;}
    int get_number_times_evaluated(){return rewards.size();}
    void reset_rewards(){rewards.clear();}
    void add_reward(double reward){rewards.push_back(reward);}
    const std::vector<double>& get_rewards(){return copy_rewards;}
    void compute_fitness();
private:
    void createMorphology() override;
    void createController() override;

    std::vector<bool> testRes;
    double manScore;
    std::vector<std::vector<std::vector<int>>> graphMatrix;
    Eigen::VectorXd morphDesc; // <width,depth,height,voxels,wheels,sensor,joint,caster>
    Eigen::VectorXd symDesc;

    bool no_actuation = false;
    bool no_sensors = false;


    double energy_cost;
    std::vector<std::vector<waypoint>> trajectories;
    double sim_time;
    std::vector<double> final_position;
    int nbr_dropped_eval = 0;

    ControllerArchive controller_archive;

    Eigen::MatrixXi visited_zones;
    DescriptorType descriptor_type = FINAL_POSITION;
    std::vector<double> rewards;
    std::vector<double> copy_rewards;

};



class M_NIPES: public EA
{
public:
    typedef std::unique_ptr<M_NIPES> Ptr;
    typedef std::unique_ptr<const M_NIPES> ConstPtr;

    M_NIPES() : EA(){}
    M_NIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}

    void init() override;
//    void epoch() override{
//        population.clear();
//        if(learning_pool.empty())
//            reproduction();
//    }
    void init_morph_pop();
    void init_next_pop() override;
    bool update(const Environment::Ptr &) override;
    bool finish_eval(const Environment::Ptr &env) override;
    bool is_finish() override;

    Individual::Ptr getIndividual(size_t index) const override{
        return population[corr_indexes[index]];
    }

    void setObjectives(size_t indIndex, const std::vector<double> &objectives)
    {
        if(simulator_side)
            std::dynamic_pointer_cast<M_NIPESIndividual>(population[indIndex])->add_reward(objectives[0]);
        currentIndIndex = indIndex;
        population[corr_indexes[indIndex]]->setObjectives(objectives);
    }

    const std::vector<genome_t>& get_gene_pool() const {return gene_pool;}
    const std::vector<learner_t>& get_learning_pool() const {return learning_pool;}
    const ControllerArchive::controller_archive_t& get_controller_archive() const {return controller_archive.archive;}
    const ControllerArchive& get_controller_archive_obj() const {return controller_archive;}

    size_t get_pop_size() const override {return corr_indexes.size();}

    genome_t &find_gene(int id);
    learner_t &find_learner(int id);

private:

    void init_new_learner(CMAESLearner &learner, const int wheel_nbr, int joint_nbr, int sensor_nbr);
    void init_new_ctrl_pop(learner_t &gene);
    void remove_oldest_gene();
    void remove_learner(int id);
    void increment_age();
    void clean_learning_pool();
    void reproduction();

    std::vector<int> corr_indexes;

    fitness_fct_t fitness_fct;

    selection_fct_t selection_fct;

    std::vector<genome_t> gene_pool;
    std::vector<learner_t> learning_pool;
    ControllerArchive controller_archive;
    int highest_age = 0;

    float current_ind_past_pos[3];
    int move_counter = 0;

    bool warming_up = true; //whether the algorithm is initialisation phase.
    bool is_multi_target = false;
};



}

#endif //M_NIPES_HPP
