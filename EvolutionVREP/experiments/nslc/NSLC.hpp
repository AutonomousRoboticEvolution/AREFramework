#ifndef NSLC_HPP
#define NSLC_HPP

#include "NSGA2.hpp"
#include "ARE/Novelty.hpp"
#include "NNIndividual.hpp"
#include "mazeEnv.h"

namespace are {

class NSLCIndividual : public NSGAIndividual, public NNIndividual
{
public:
    typedef std::shared_ptr<NSLCIndividual> Ptr;
    typedef std::shared_ptr<const NSLCIndividual> ConstPtr;

    NSLCIndividual() : NNIndividual(), NSGAIndividual(){}
    NSLCIndividual(const EmptyGenome::Ptr& morph_gen,const NNGenome::Ptr& ctrl_gen) :
        NNIndividual(morph_gen, ctrl_gen){}
    NSLCIndividual(const NSLCIndividual& ind) :
        NNIndividual(ind),
        NSGAIndividual(ind){}


    Individual::Ptr clone(){
        return std::make_shared<NSLCIndividual>(*this);
    }

    Individual::Ptr crossover(const Individual::Ptr &){
        return clone();
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

    void mutation(){
        for(Individual::Ptr &ind : population)
            std::dynamic_pointer_cast<NNGenome>(ind->get_ctrl_genome())->mutate(rng);
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



#endif
