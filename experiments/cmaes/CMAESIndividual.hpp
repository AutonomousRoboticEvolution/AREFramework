#ifndef CMAESINDIVIDUAL_HPP
#define CMAESINDIVIDUAL_HPP

#include <ARE/Individual.h>
#include <ARE/Genome.h>
#include "AREPuckMorphology.h"
#include "EPuckMorphology.h"
#include "NNControl.h"
#include "NNGenome.hpp"
#include "NNParamGenome.hpp"

namespace are {



class CMAESIndividual : public Individual
{
public:
    CMAESIndividual() : Individual(){}
    CMAESIndividual(const EmptyGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    CMAESIndividual(const CMAESIndividual& ind) :
        Individual(ind),
        final_position(ind.final_position){}

    Individual::Ptr clone() override {
        return std::make_shared<CMAESIndividual>(*this);
    }

    void update(double delta_time) override;

    std::string to_string();
    void from_string(const std::string&);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & final_position;
    }

    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}

private:
    void createMorphology() override;
    void createController() override;

    std::vector<double> final_position;
};

}
#endif //CMAESINDIVIDUAL_HPP
