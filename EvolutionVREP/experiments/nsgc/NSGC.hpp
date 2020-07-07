#ifndef NSGC_HPP
#define NSGC_HPP

#include "ARE/learning/NSGA2.hpp"
#include "ARE/learning/Novelty.hpp"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/NNParamGenome.hpp"
#include "ARE/mazeEnv.h"

namespace are {

class NSGCIndividual : public NSGAIndividual<NSGCIndividual>, public NN2Individual
{
public:
    typedef std::shared_ptr<NSGCIndividual> Ptr;
    typedef std::shared_ptr<const NSGCIndividual> ConstPtr;

    NSGCIndividual() : NN2Individual(), NSGAIndividual(){}
    NSGCIndividual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        NN2Individual(morph_gen, ctrl_gen){}
    NSGCIndividual(const NSGCIndividual& ind) :
        NN2Individual(ind),
        NSGAIndividual(ind){}


    Individual::Ptr clone() override{
        return std::make_shared<NSGCIndividual>(*this);
    }

    void crossover(const Individual::Ptr& partner, Individual& child1, Individual& child2){
        NN2Individual::crossover(partner,child1,child2);
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

class NSGC : public NSGA2<NSGCIndividual>
{
public:
    NSGC() : NSGA2<NSGCIndividual>(){}
    NSGC(const settings::ParametersMapPtr& param) : NSGA2<NSGCIndividual>(param){}

    void init() override;

    void epoch() override;

    bool update(const Environment::Ptr&) override;

    void mutation(){
        for(Individual::Ptr &ind : population)
            ind->mutate();
    }

    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;

    bool is_finish() override;

    const std::vector<Eigen::VectorXd> &get_archive(){return archive;}

private:
    std::vector<Eigen::VectorXd> archive;
    double best_fitness;
    int currentIndIndex;
};

}



#endif //NSGC_HPP
