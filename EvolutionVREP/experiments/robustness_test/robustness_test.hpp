#ifndef ROBUSTNESS_TEST_HPP
#define ROBUSTNESS_TEST_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <ARE/EA.h>
#include <ARE/Logging.h>
#include <ARE/Individual.h>
#include "NNParamGenome.hpp"
#include "NNGenome.hpp"
#include "NNControl.h"
#include "NN2Individual.hpp"
#include "FixedMorphology.hpp"
#include "settings.hpp"


namespace are {

class RobustnessTest : public EA
{
public:
    RobustnessTest() : EA(){}
    RobustnessTest(const settings::ParametersMapPtr& param) : EA(param){}
    ~RobustnessTest(){
    }

    void init();
};

}
#endif //ROBUSTNESS_TEST_HPP
