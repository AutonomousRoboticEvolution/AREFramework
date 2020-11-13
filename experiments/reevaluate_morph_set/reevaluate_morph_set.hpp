#ifndef REEVALUATE_MORPH_SET_HPP
#define REEVALUATE_MORPH_SET_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "ARE/CPPNGenome.h"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/Morphology_CPPNMatrix.h"
#include "ARE/Settings.h"'


namespace are {

class ReevaluateMorphSet : public EA
{
public:
    ReevaluateMorphSet() : EA(){}
    ReevaluateMorphSet(const settings::ParametersMapPtr& param) : EA(param){}
    ~ReevaluateMorphSet(){
    }

    void init();
    bool update(const Environment::Ptr &) override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives);

    void load_population(const std::string &folder);

private:
    int currentIndIndex;
};

}
#endif //REEVALUATE_MORPH_SET_HPP

