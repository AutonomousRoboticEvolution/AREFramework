#ifndef BLOB_TRACKER_HPP
#define BLOB_TRACKER_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Settings.h"
#include "physicalER/pi/pi_individual.hpp"

namespace are{

class RandomController : public EA
{
public:
    typedef std::shared_ptr<RandomController> Ptr;
    typedef std::shared_ptr<const RandomController> ConstPtr;

    RandomController(){}
    RandomController(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    void init() override;
    void init_pop();
    void init_next_pop() override;
    void load_data_for_update() override;

private:
    NNParamGenome::Ptr makeRandomController(int numberOfInputs, int numberOfOutputs);
};

}//are

#endif
