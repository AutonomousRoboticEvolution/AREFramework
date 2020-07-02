#ifndef NNINDIVIDUAL_HPP
#define NNINDIVIDUAL_HPP

#include "ARE/Individual.h"
#include "NNGenome.hpp"
#include "ARE/FixedMorphology.hpp"
#include "NNControl.h"

namespace are {

class NNIndividual : public Individual
{
public:
    typedef std::shared_ptr<NNIndividual> Ptr;
    typedef std::shared_ptr<const NNIndividual> ConstPtr;

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

//    NNGenome::Ptr &access_ctrl_genome(){return NNG}

    std::string to_string() override;
    void from_string(const std::string&) override;


    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
    }

protected:
    void createMorphology() override;
    void createController() override;

    std::vector<double> final_position;
};

}


#endif //NNINDIVIDUAL_HPP
