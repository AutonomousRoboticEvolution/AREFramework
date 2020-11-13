#ifndef M_NIPES_HPP
#define M_NIPES_HPP

#include <boost/filesystem.hpp>

#include "ARE/CPPNGenome.h"
#include "ARE/learning/NIPES.hpp"
#include "ARE/Morphology_CPPNMatrix.h"
#include "cmaes_learner.hpp"


namespace are{

using CPPNMorph = Morphology_CPPNMatrix;

typedef
/**
 * @brief Archive of controller ordered in a 3D vector with the following dimension : number of wheels, number of joints, number of sensors
 *      The archive store pairs of controller genome and associated fitness.
 */
struct ControllerArchive{
    typedef std::vector<std::vector<std::vector<std::pair<NNParamGenome::Ptr,double>>>> controller_archive_t;
    controller_archive_t archive;

    void init(int max_wheels,int max_joints, int max_sensors);

    /**
     * @brief update the archive. If the fitness of the candidate controller is greater than the stored one.
     * @param candidate new genome
     * @param associate fitness
     * @param number of wheels
     * @param number of joints
     * @param number of sensors
     */
    void update(const NNParamGenome::Ptr& genome, double fitness, int wheels, int joints, int sensors);

    template<class archive_t>
    void serialize(archive_t &arch, const unsigned int v)
    {
        arch & archive;
    }


} ControllerArchive;

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

    void setGenome();

    void setManRes();
    void setManScore();

    /// Setters for descritors
    void setMorphDesc();
    void setGraphMatrix();
    void setSymDesc();

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
    }

    std::string to_string() override;
    void from_string(const std::string &) override;


    Eigen::VectorXd getMorphDesc(){return  morphDesc;}

    void set_ctrl_archive(const ControllerArchive& ctrl_arc){controller_archive = ctrl_arc;}

private:
    void createMorphology() override;
    void createController() override;

    NEAT::NeuralNetwork nn;
    std::vector<bool> testRes;
    double manScore;
    std::vector<std::vector<std::vector<int>>> graphMatrix;
    Eigen::VectorXd morphDesc;
    Eigen::VectorXd symDesc;

    double energy_cost;
    std::vector<waypoint> trajectory;
    double sim_time;
    std::vector<double> final_position;

    int nn_inputs;
    int nn_outputs;

    ControllerArchive controller_archive;
};

class M_NIPES : public EA
{
public:

    typedef std::unique_ptr<M_NIPES> Ptr;
    typedef std::unique_ptr<const M_NIPES> ConstPtr;

    M_NIPES() : EA(){}
    M_NIPES(const settings::ParametersMapPtr& param) : EA(param){}

    void init() override;
    void init_morph_pop();
    void init_next_pop() override;
    void epoch() override;
    bool update(const Environment::Ptr &) override;
//    bool is_finish() override;
    bool finish_eval() override;

    //GETTERS
    const std::string& subFolder(){return sub_folder;}

private:
    typedef struct morph_desc_t{
        int wheels;
        int joints;
        int sensors;
    }morph_desc_t;

    void loadNEATGenome(short int genomeID, NEAT::Genome& gen);
    void listMorphGenomeID(std::vector<short int>& list);
    void loadNbrSenAct(const std::vector<short int>& list, std::map<short int, morph_desc_t>& desc_map);

    std::vector<short int> morphIDList;
    std::map<short,morph_desc_t> morphDescMap;
    int morphCounter = 0;
    std::string sub_folder;
    NEAT::Genome current_morph_gen;
    NEAT::Parameters neat_params;
    std::unique_ptr<NEAT::Population> morph_population;

    ControllerArchive controller_archive;

    float current_ind_past_pos[3];
    int move_counter = 0;
    int nbr_dropped_eval = 0;
};

}



#endif //PMNIPES_HPP
