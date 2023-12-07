#ifndef REEVALUATE_FIXED_HPP
#define REEVALUATE_FIXED_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "simulatedER/FixedMorphology.hpp"
#include "ARE/Settings.h"
#include "simulatedER/obstacleAvoidance.hpp"

namespace are {

class ReevaluateFixed : public EA
{
public:
    ReevaluateFixed() : EA(){}
    ReevaluateFixed(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~ReevaluateFixed(){
    }

    void init() override;
    bool update(const Environment::Ptr &) override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;

private:
    int currentIndIndex;
};

}
#endif //REEVALUATE_FIXED_HPP

