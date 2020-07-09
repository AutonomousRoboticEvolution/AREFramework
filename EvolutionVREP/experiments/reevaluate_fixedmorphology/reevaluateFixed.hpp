#ifndef REEVALUATE_FIXED_HPP
#define REEVALUATE_FIXED_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/FixedMorphology.hpp"
#include "ARE/Settings.h"'


namespace are {

class ReevaluateFixed : public EA
{
public:
    ReevaluateFixed() : EA(){}
    ReevaluateFixed(const settings::ParametersMapPtr& param) : EA(param){}
    ~ReevaluateFixed(){
    }

    void init();
    bool update(const Environment::Ptr &) override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives);

private:
    int currentIndIndex;
};

}
#endif //REEVALUATE_FIXED_HPP

