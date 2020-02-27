#ifndef NSGAINDIVIDUAL_HPP
#define NSGAINDIVIDUAL_HPP

#include "ARE/Individual.h"
#include "NNGenome.hpp"
#include "EPuckMorphology.h"
#include "NNControl.h"

namespace are {


struct NSGAInfo
{
    std::vector<Individual::Ptr> dominated; //List of dominated individual by this
    int domination_counter; //number of individuals who dominates this
    float crowd_dist;
    int rank;
};

class NNIndividual : public Individual
{
public:
    NNIndividual() : Individual(){}
    NNIndividual(const EmptyGenome::Ptr& morph_gen,const NNGenome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    NNIndividual(const NNIndividual& ind) :
        Individual(ind),
        final_position(ind.final_position){}

    Individual::Ptr clone() override {
        return std::make_shared<NNIndividual>(*this);
    }

    void update(double delta_time) override;

    void set_final_position(const std::vector<double> fp){final_position = fp;}
    const std::vector<double> get_final_position(){return final_position;}

    Individual::Ptr crossover(const Individual::Ptr &){
        //no crossover, just simple cloning
        return std::dynamic_pointer_cast<NNIndividual>(clone());
    }

//    NNGenome::Ptr &access_ctrl_genome(){return NNG}

    std::string to_string();
    void from_string(const std::string&);


    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
    }

    const NSGAInfo &get_nsga_info(){return nsga_info;}
    NSGAInfo &access_nsga_info(){return nsga_info;}

protected:
    void createMorphology() override;
    void createController() override;

    std::vector<double> final_position;

    NSGAInfo nsga_info;
};


}


#endif //NSGAINDIVIDUAL_HPP
