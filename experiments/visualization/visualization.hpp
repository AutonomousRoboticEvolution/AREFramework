#ifndef VISU_POP_HPP
#define VISU_POP_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/CPPNGenome.h"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/Settings.h"


namespace are {

using CPPNMorph = sim::Morphology_CPPNMatrix;

class VisuInd : public Individual
{
public:
    VisuInd(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen)
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
    Visu(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~Visu(){
    }

    void init() override;
    bool is_finish() override;
    void load_per_gen_ind(int indIdx, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);
    void load_per_id(int id, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);

};

}
#endif //VISU_POP_HPP
