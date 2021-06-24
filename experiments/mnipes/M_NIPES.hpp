#ifndef M_NIPES_HPP
#define M_NIPES_HPP

#include "ARE/CPPNGenome.h"
#include "ARE/learning/ipop_cmaes.hpp"
#include "ARE/learning/Novelty.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "cmaes_learner.hpp"
#include "ARE/misc/eigen_boost_serialization.hpp"
#include <multineat/Population.h>
#include "ARE/learning/controller_archive.hpp"
#include "obstacleAvoidance.hpp"

namespace are{

using CPPNMorph = sim::Morphology_CPPNMatrix;

typedef std::function<double(const CMAESLearner::Ptr&)> fitness_fct_t;

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
/**
 * @brief The M_NIPESIndividual class
 */
class M_NIPESIndividual : public Individual
{
public:
    M_NIPESIndividual() :  Individual(){}
    M_NIPESIndividual(const CPPNGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen,const CMAESLearner::Ptr& cma_learner)
        : Individual(morph_gen,ctrl_gen)
    {
        learner = cma_learner;
    }
    M_NIPESIndividual(const M_NIPESIndividual& ind) :
        Individual(ind),
        nn(ind.nn),
        testRes(ind.testRes),
        graphMatrix(ind.graphMatrix),
        morphDesc(ind.morphDesc),
        symDesc(ind.symDesc),
        nn_inputs(ind.nn_inputs),
        nn_outputs(ind.nn_outputs),
        final_position(ind.final_position),
        trajectory(ind.trajectory),
        energy_cost(ind.energy_cost),
        sim_time(ind.sim_time),
        controller_archive(ind.controller_archive)
    {}

    Individual::Ptr clone() override {
        return std::make_shared<M_NIPESIndividual>(*this);
    }

    void update(double delta_time) override;

    //specific to the current ARE arenas
    Eigen::VectorXd descriptor();
    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}
    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}
    double get_energy_cost(){return energy_cost;}
    double get_sim_time(){return sim_time;}
    void set_ctrl_genome(const NNParamGenome::Ptr &gen){std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome) = gen;}

    void setGenome();

    void setManRes();

    /// Setters for descritors
    void setMorphDesc();

    void set_nn_inputs(int nni){nn_inputs = nni;}
    void set_nn_outputs(int nno){nn_outputs = nno;}


    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & learner;
        arch & morphGenome;
        arch & ctrlGenome;
        arch & final_position;
        arch & energy_cost;
        arch & trajectory;
        arch & sim_time;
        arch & nn_inputs;
        arch & nn_outputs;
        arch & controller_archive;
        arch & morphDesc;
        arch & visited_zones;

    }

    std::string to_string() override;
    void from_string(const std::string &) override;


    Eigen::VectorXd getMorphDesc(){return  morphDesc;}

    void set_ctrl_archive(const ControllerArchive& ctrl_arc){controller_archive = ctrl_arc;}

    bool is_actuated(){return !no_actuation;}
    bool has_sensor(){return !no_sensors;}
    void set_visited_zones(const Eigen::MatrixXi& vz){visited_zones = vz;}
    void set_descriptor_type(DescriptorType dt){descriptor_type = dt;}



private:
    void createMorphology() override;
    void createController() override;

    NEAT::NeuralNetwork nn;
    std::vector<bool> testRes;
    double manScore;
    std::vector<std::vector<std::vector<int>>> graphMatrix;
    Eigen::VectorXd morphDesc; // <width,depth,height,voxels,wheels,sensor,joint,caster>
    Eigen::VectorXd symDesc;

    bool no_actuation = false;
    bool no_sensors = false;

    double energy_cost;
    std::vector<waypoint> trajectory;
    double sim_time;
    std::vector<double> final_position;

    int nn_inputs;
    int nn_outputs;

    ControllerArchive controller_archive;

    Eigen::MatrixXi visited_zones;
    DescriptorType descriptor_type = FINAL_POSITION;
};

class M_NIPES : public EA
{
public:

    typedef std::unique_ptr<M_NIPES> Ptr;
    typedef std::unique_ptr<const M_NIPES> ConstPtr;

    M_NIPES() : EA(){}
    M_NIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}

    void init() override;
    void init_morph_pop();
    void init_next_pop() override;
    void epoch() override;
    bool update(const Environment::Ptr &) override;
//    bool is_finish() override;
    bool finish_eval(const Environment::Ptr &) override;
    bool update_maze(const Environment::Ptr& env);
    bool update_obstacle_avoidance(const Environment::Ptr& env);
    //GETTERS
    const std::string& subFolder(){return sub_folder;}

    const ControllerArchive::controller_archive_t& get_controller_archive(){
        return controller_archive.archive;
    }

    void set_controller_archive(const ControllerArchive::controller_archive_t archive){
        controller_archive.archive = archive;
    }

private:
    typedef struct morph_desc_t{
        int wheels;
        int joints;
        int sensors;
    }morph_desc_t;

    void loadNEATGenome(short int genomeID, NEAT::Genome& gen);
    void listMorphGenomeID(std::vector<short int>& list);
    void loadNbrSenAct(const std::vector<short int>& list, std::map<short int, morph_desc_t>& desc_map);
    void loadControllerArchive(const std::string &file);
    int findLastGen(const std::string &exp_folder);

    std::vector<short int> morphIDList;
    int morphCounter = 0;
    std::string sub_folder;
    NEAT::Genome current_morph_gen;
    NEAT::Parameters neat_params;
    std::unique_ptr<NEAT::Population> morph_population;

    ControllerArchive controller_archive;

    fitness_fct_t fitness_fct;

    float current_ind_past_pos[3];
    int move_counter = 0;
    int nbr_dropped_eval = 0;
    bool learning_finished = false;

};

}



#endif //PMNIPES_HPP
