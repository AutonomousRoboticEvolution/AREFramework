#ifndef VISU_POP_HPP
#define VISU_POP_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/Morphology_CPPNMatrix.h"
#include "ARE/CPPNGenome.h"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/Settings.h"


namespace are {

using CPPNMorph = Morphology_CPPNMatrix;

class VisuInd : public Individual
{
public:
    VisuInd(const CPPNGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {
    }
    VisuInd(const VisuInd& ind) :
        Individual(ind)
    {}
    Individual::Ptr clone() override{
        return std::make_shared<VisuInd>(*this);
    };
    void update(double delta_time) override;


private:
    void createMorphology() override;
    void createController() override;

};

class Visu : public EA
{
public:
    Visu() : EA(){}
    Visu(const settings::ParametersMapPtr& param) : EA(param){}
    ~Visu(){
    }

    void init();

};

}
#endif //VISU_POP_HPP
