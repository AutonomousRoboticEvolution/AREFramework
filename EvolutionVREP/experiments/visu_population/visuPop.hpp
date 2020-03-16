#ifndef VISU_POP_HPP
#define VISU_POP_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <ARE/EA.h>
#include <ARE/Logging.h>
#include <ARE/Individual.h>
#include "NNParamGenome.hpp"
#include "NNGenome.hpp"
#include "NNControl.h"
#include "EPuckMorphology.h"
#include "AREPuckMorphology.h"
#include "settings.hpp"


namespace are {


class VisuIndividual : public Individual
{
public:
    VisuIndividual() : Individual(){}
    VisuIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    VisuIndividual(const VisuIndividual& ind) :
        Individual(ind),
        final_position(ind.final_position){}

    Individual::Ptr clone() override {
        return std::make_shared<VisuIndividual>(*this);
    }

    void update(double delta_time) override;

    void set_final_position(const std::vector<double> fp){final_position = fp;}
    const std::vector<double> get_final_position(){return final_position;}



protected:
    void createMorphology() override;
    void createController() override;

    std::vector<double> final_position;
};

class VisuPop : public EA
{
public:
    VisuPop() : EA(){}
    VisuPop(const settings::ParametersMapPtr& param) : EA(param){}
    ~VisuPop(){
    }

    void init();

};

}
#endif //VISU_POP_HPP
