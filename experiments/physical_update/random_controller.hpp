#ifndef BLOB_TRACKER_HPP
#define BLOB_TRACKER_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Settings.h"
#include "physicalER/pi_individual.hpp"
#include "physicalER/io_helpers.hpp"

namespace are{

class RandomController : public EA
{
public:
    typedef std::shared_ptr<RandomController> Ptr;
    typedef std::shared_ptr<const RandomController> ConstPtr;

    RandomController(){}
    RandomController(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    void init() override{}
    void init_next_pop() override{}
    bool update(const Environment::Ptr &) override;
    void load_data_for_update() override;
    void write_data_for_update() override;

    virtual const Genome::Ptr get_next_controller_genome(int id);

private:
    const NNParamGenome::Ptr &make_random_ctrl(int wheels, int joints, int sensors);
    std::vector<std::string> ids;
    std::vector<std::vector<waypoint>> trajectories;
};

}//are

#endif
