#ifndef ROBUSTNESS_TEST_HPP
#define ROBUSTNESS_TEST_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/FixedMorphology.hpp"



namespace are {

class RobustnessTest : public EA
{
public:
    RobustnessTest() : EA(){}
    RobustnessTest(const settings::ParametersMapPtr& param) : EA(param){}
    ~RobustnessTest(){
    }

    void init();
    bool update(const Environment::Ptr &) override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives);

private:
    int currentIndIndex;
};

}
#endif //ROBUSTNESS_TEST_HPP
