#ifndef NSLC_HPP
#define NSLC_HPP

#include "ARE/learning/NSGA2.hpp"
#include "ARE/learning/Novelty.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/NNParamGenome.hpp"
#include "simulatedER/mazeEnv.h"

namespace are {

class NSLCIndividual : public NSGAIndividual<NSLCIndividual>, public sim::NN2Individual
{
public:
    typedef std::shared_ptr<NSLCIndividual> Ptr;
    typedef std::shared_ptr<const NSLCIndividual> ConstPtr;

    NSLCIndividual() : sim::NN2Individual(), NSGAIndividual(){}
    NSLCIndividual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        sim::NN2Individual(morph_gen, ctrl_gen){}
    NSLCIndividual(const NSLCIndividual& ind) :
        sim::NN2Individual(ind),
        NSGAIndividual(ind){}


    Individual::Ptr clone() override{
        return std::make_shared<NSLCIndividual>(*this);
    }

    void crossover(const Individual::Ptr& partner, Individual& child1, Individual& child2){
        sim::NN2Individual::crossover(partner,child1,child2);
    }

    //specific to the current ARE arenas
    Eigen::VectorXd descriptor(){
        Eigen::VectorXd desc(3);
        desc << (final_position[0]+1)/2., (final_position[1]+1)/2., (final_position[2]+1)/2.;
        return desc;
    }
    void addObjective(double obj){objectives.push_back(obj);}



    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
    }
};

class NSLC : public NSGA2<NSLCIndividual>
{
public:
    typedef std::shared_ptr<NSLC> Ptr;
    typedef std::shared_ptr<const NSLC> ConstPtr;

    NSLC() : NSGA2<NSLCIndividual>(){}
    NSLC(const settings::ParametersMapPtr& param) : NSGA2<NSLCIndividual>(param){}

    void init() override;

    void epoch() override;

    bool update(const Environment::Ptr&) override;

    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;

    bool is_finish() override;

    const std::vector<Eigen::VectorXd> &get_archive(){return archive;}

private:
    std::vector<Eigen::VectorXd> archive;
    double best_fitness;
    int currentIndIndex;
};

}



#endif
