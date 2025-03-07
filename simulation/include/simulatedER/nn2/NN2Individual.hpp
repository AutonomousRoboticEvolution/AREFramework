#ifndef NN2INDIVIDUAL_HPP
#define NN2INDIVIDUAL_HPP

#include "ARE/Individual.h"
#include "ARE/Genome.h"
#include "simulatedER/FixedMorphology.hpp"
#include "simulatedER/mazeEnv.h"
#include "ARE/nn2/NN2Control.hpp"
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/Settings.h"


namespace are {

namespace sim{


class NN2Individual : public Individual
{
public:

    typedef std::shared_ptr<NN2Individual> Ptr;
    typedef std::shared_ptr<const NN2Individual> ConstPtr;

    NN2Individual() : Individual(){}
    NN2Individual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    NN2Individual(const NN2Individual& ind) :
        Individual(ind),
        final_position(ind.final_position),
        trajectory(ind.trajectory),
        energy_cost(ind.energy_cost),
        sim_time(ind.sim_time)
    {}

    Individual::Ptr clone() override {
        return std::make_shared<NN2Individual>(*this);
    }

    void update(double delta_time) override;

    //specific to the current ARE arenas
    Eigen::VectorXd descriptor() override{
        double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
        Eigen::VectorXd desc(3);
        desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
        return desc;
    }

    void addObjective(double obj){objectives.push_back(obj);}

    void crossover(const Individual::Ptr &partner, Individual* child) override;
    void symmetrical_crossover(const Individual::Ptr &partner, Individual* child1, Individual *child2) override;


    std::string to_string() const override;
    void from_string(const std::string&) override;

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
      //  arch & energy_cost;
        arch & trajectory;
      //  arch & sim_time;
    }

    //GETTERS & SETTERS
    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}
    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}
    double get_energy_cost(){return energy_cost;}
    double get_sim_time(){return sim_time;}


protected:
    void createMorphology() override;
    void createController() override;
    double sum_ctrl_freq = 0;

    std::vector<double> final_position;
    std::vector<waypoint> trajectory;
    double energy_cost=0;
    double sim_time=0;
};

}//sim

}//are
#endif //NN2INDIVIDUAL_HPP
