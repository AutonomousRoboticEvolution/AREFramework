#ifndef M_NIPES_HPP
#define M_NIPES_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <multineat/Population.h>
#include "ARE/EA.h"
#include "ARE/Settings.h"
#include "ARE/learning/Novelty.hpp"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/CPPNGenome.h"
#include "physicalER/pi_individual.hpp"
#include "physicalER/io_helpers.hpp"
#include "cmaes_learner.hpp"
#include "ARE/learning/controller_archive.hpp"
#include "ARE/crossbreeding.hpp"
#include "ProtomatrixGenome.hpp"

namespace are{

typedef enum GenomeType{
    CPPN = 0,
    PROTOMATRIX = 1
} GenomeType;

class PMEIndividual : public Individual
{
public:
    typedef std::shared_ptr<PMEIndividual> Ptr;
    typedef std::shared_ptr<const PMEIndividual> ConstPtr;

    PMEIndividual() : Individual(){}
    PMEIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen){
        this->morphGenome = morph_gen;
        this->ctrlGenome = ctrl_gen;
    }

    PMEIndividual(const PMEIndividual& ind) :
        Individual(ind)
    {}

    Individual::Ptr clone() override
    {return std::make_shared<PMEIndividual>(*this);}

    void update(double delta_time) override {}


    const Eigen::VectorXd &get_morphDesc() const {return morphDesc;}
    const std::vector<int> &getListOrganTypes() const {return listOrganTypes;}
    const std::vector<std::vector<float>> &getListOrganPos() const {return listOrganPos;}
    const std::vector<std::vector<float>> &getListOrganOri() const {return listOrganOri;}
    const std::vector<float> &getSkeletonListVertices() const {return skeletonListVertices;}
    const std::vector<int> &getSkeletonListIndices() const {return skeletonListIndices;}

private:
    void createMorphology() override;
    void createController() override{}

    nn2_cppn_t cppn;
    Eigen::VectorXd morphDesc;
    std::vector<bool> testRes;
    std::vector<int> listOrganTypes;
    std::vector<std::vector<float>> listOrganPos;
    std::vector<std::vector<float>> listOrganOri;
    std::vector<float> skeletonListVertices;
    std::vector<int> skeletonListIndices;

};

class MNIPES : public EA
{
public:
    typedef std::shared_ptr<MNIPES> Ptr;
    typedef std::shared_ptr<const MNIPES> ConstPtr;

    MNIPES(){}
    MNIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param);
    void init() override;
    void init_random_pop();
    void init_next_pop() override;

    bool update(const Environment::Ptr &env) override;
    void setObjectives(size_t current_id,const std::vector<double> &objs){}
    void init_learner(int id);
    const Genome::Ptr get_next_controller_genome(int id);

    void load_data_for_generate() override;
    void write_data_for_generate() override;
    void load_data_for_update() override;
    void write_data_for_update() override;

    int get_currentID(){return currentIndIndex;}
    const std::vector<waypoint> &get_trajectory(){return trajectory;}
    std::vector<double> get_objectives(){return objectives;}
    const std::map<int, CMAESLearner> &get_learners(){return learners;}
    const NNParamGenome &get_best_current_ctrl_genome() const {return best_current_ctrl_genome;}
private:
    std::vector<std::pair<int,int>> ids;
    std::map<int,Genome::Ptr> morph_genomes;
    std::map<int,Genome::Ptr> xmigrator_genomes; //genomes from simulation to be directly introduced in physical population
    std::map<int,CMAESLearner> learners;
    std::vector<waypoint> trajectory;
    std::vector<double> objectives;
    ControllerArchive ctrl_archive;
    ioh::MorphGenomeInfoMap morph_genomes_info;
    NNParamGenome best_current_ctrl_genome;

    void _survival(const ioh::MorphGenomeInfoMap& morph_gen_info, std::vector<int>& list_ids);
    void _reproduction();

    void write_morph_descriptors();
};

}//are

#endif //M_NIPES_HPP
