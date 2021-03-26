#ifndef MONIPES_HPP
#define MONIPES_HPP

#include "NIPES.hpp"
#include "NSGA2.hpp"
#include "NN2Individual.hpp"

namespace are{

class MONN2Individual : public NN2Individual, public NSGAIndividual
{
public :
    typedef std::shared_ptr<MONN2Individual> Ptr;
    typedef std::shared_ptr<const MONN2Individual> ConstPtr;

    MONN2Individual() : NN2Individual(), NSGAIndividual(){}
    MONN2Individual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        NN2Individual(morph_gen, ctrl_gen){}
    MONN2Individual(const MONN2Individual& ind) :
        NN2Individual(ind),
        NSGAIndividual(ind){}


    Individual::Ptr clone(){
        return std::make_shared<MONN2Individual>(*this);
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

class MONIPES : public NIPES, public NSGA2<MONN2Individual>
{
public:
    MONIPES() : NIPES(), NSGA2<MONN2Individual>(){}
    MONIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    void init() override;
    void epoch() override;
    bool update(const Environment::Ptr&) override;

    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;
    bool is_finish() override;
};
}

#endif //MONIPES_HPP
