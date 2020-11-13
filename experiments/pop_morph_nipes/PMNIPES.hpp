#ifndef PMNIPES_HPP
#define PMNIPES_HPP

#include <boost/filesystem.hpp>

#include "ARE/CPPNGenome.h"
#include "ARE/learning/NIPES.hpp"
#include "ARE/Morphology_CPPNMatrix.h"


namespace are{

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

class PMNIPESIndividual : public NN2Individual
{
public:
    PMNIPESIndividual() : NN2Individual(){}
    PMNIPESIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        NN2Individual(morph_gen,ctrl_gen){}
    PMNIPESIndividual(const PMNIPESIndividual& ind) :
        NN2Individual(ind),
        nn(ind.nn),
        testRes(ind.testRes),
        graphMatrix(ind.graphMatrix),
        morphDesc(ind.morphDesc),
        symDesc(ind.symDesc),
        nn_inputs(ind.nn_inputs),
        nn_outputs(ind.nn_outputs)
    {}

    void update(double delta_time) override;

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
        arch & ctrlGenome;
        arch & morphGenome;
        arch & final_position;
//        arch & energy_cost;
        arch & trajectory;
        arch & sim_time;
        arch & nn_inputs;
        arch & nn_outputs;
    }

    std::string to_string() override;
    void from_string(const std::string &) override;

private:
    void createMorphology() override;
    void createController() override;

    NEAT::NeuralNetwork nn;
    std::vector<bool> testRes;
    double manScore;
    std::vector<std::vector<std::vector<int>>> graphMatrix;
    Eigen::VectorXd morphDesc;
    Eigen::VectorXd symDesc;


    int nn_inputs;
    int nn_outputs;

};

class PMNIPES : public NIPES
{
public:

    typedef std::unique_ptr<PMNIPES> Ptr;
    typedef std::unique_ptr<const PMNIPES> ConstPtr;

    PMNIPES() : NIPES(){}
    PMNIPES(const settings::ParametersMapPtr& param) : NIPES(param){}

    void init() override;
    void init_cmaes(int dim);
    void init_first_pop(int nbr_weights, int nbr_bias);
    void init_next_pop() override;
    bool update(const Environment::Ptr &) override;
    bool is_finish() override;

    bool finish_eval() override;

    //GETTERS
    const std::string& subFolder(){return sub_folder;}
    const ControllerArchive::controller_archive_t& get_controller_archive(){return controller_archive.archive;}

private:
    typedef struct morph_desc_t{
        int wheels;
        int joints;
        int sensors;
    }morph_desc_t;

    void loadNEATGenome(short int genomeID, NEAT::Genome& gen);
    void listMorphGenomeID(std::vector<short int>& list, const std::vector<short> &past_list = std::vector<short>());
    void loadNbrSenAct(std::vector<short> &list, std::map<short int, morph_desc_t>& desc_map);

    std::vector<short int> morphIDList;
    std::map<short,morph_desc_t> morphDescMap;
    int morphCounter = 0;
    std::string sub_folder;
    NEAT::Genome current_morph_gen;
    float current_ind_past_pos[3];
    int move_counter = 0;
    int nbr_dropped_eval = 0;
    ControllerArchive controller_archive;
};

}



#endif //PMNIPES_HPP
