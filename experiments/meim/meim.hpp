#ifndef MEIM_HPP
#define MEIM_HPP

#include <ARE/EA.h>
#include <ARE/nn2/NN2CPPNGenome.hpp>
#include <simulatedER/Morphology_CPPNMatrix.h>
#include "homeokinesis_controller.hpp"
#include "obstacleAvoidance.hpp"


namespace are{

typedef struct genome_t{
    genome_t(NN2CPPNGenome::Ptr mg,hk::Homeokinesis::Ptr ctrl, std::vector<double> objs):
        morph_genome(mg),controller(ctrl),objectives(objs){}
    genome_t(const genome_t &g) :
        morph_genome(g.morph_genome),
        controller(g.controller),
        objectives(g.objectives),
        trajectory(g.trajectory){}
    NN2CPPNGenome::Ptr morph_genome;
    hk::Homeokinesis::Ptr controller;
    std::vector<double> objectives;
    std::vector<waypoint> trajectory;
} genome_t;


using CPPNMorph = sim::Morphology_CPPNMatrix;

class MEIMIndividual : public Individual
{
public:
    MEIMIndividual() : Individual(){}
    MEIMIndividual(const NN2CPPNGenome::Ptr& morph_gen,const EmptyGenome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {}
    MEIMIndividual(const MEIMIndividual& ind):
        Individual(ind){}


    Individual::Ptr clone() override {
        return std::make_shared<MEIMIndividual>(*this);
    }

    void update(double delta_time) override;

    std::string to_string() override;
    void from_string(const std::string &) override;

    void set_init_position(const std::vector<double>& ip){init_position = ip;}
    void set_final_position(const std::vector<double> &fp){final_position=fp;}
    void set_visited_zones(const Eigen::MatrixXi& vz){visited_zones = vz;}
    void set_trajectory(const std::vector<waypoint> traj){trajectory = traj;}
    const std::vector<waypoint> &get_trajectory() const {return trajectory;}

private:
    void createMorphology() override;
    void createController() override;

    int individual_id;
    std::vector<double> init_position;
    std::vector<double> final_position;
    Eigen::MatrixXi visited_zones;
    std::vector<waypoint> trajectory;
    int nb_sensors;
    int nb_joints;
    int nb_wheels;

};

class MEIM: public EA
{
public:
    typedef std::unique_ptr<MEIM> Ptr;
    typedef std::unique_ptr<const MEIM> ConstPtr;

    MEIM() : EA(){}
    MEIM(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param): EA(rn,param){}

    void init() override;
    void init_next_pop() override;
    bool update(const Environment::Ptr &) override;
    bool finish_eval(const Environment::Ptr &) override;
    bool is_finish() override;

    void setObjectives(size_t index, const std::vector<double> &objs) override;
    const std::vector<genome_t> &get_parent_pool() const {return parent_pool;}
    const std::vector<genome_t> &get_new_genes() const {return new_genes;}
    void clear_new_genes(){new_genes.clear();}

   void fill_ind_to_eval(std::vector<int> &ind_to_eval) override;



private:
   std::vector<int> newly_evaluated;
   std::vector<genome_t> parent_pool;
   std::vector<genome_t> new_genes;
   int highest_morph_id = 0;

   void reproduction();
   void remove_worst_parent();
   NN2CPPNGenome best_of_subset(const std::vector<genome_t>);

};
}

#endif //MEIM_HPP