#ifndef CMABO_HPP
#define CMABO_HPP

#include "NIPES.hpp"
#include "NN2Individual.hpp"
#include "BOLearner.h"

namespace are
{

using obs_fct_t = std::function<Eigen::VectorXd(const NN2Individual::Ptr &)>;

struct obs_fcts{
    /**
     * @brief Observation corresponding to the final position of the robot
     */
    static obs_fct_t final_position;

};

class CMABO : public NIPES
{

public:

    typedef std::unique_ptr<CMABO> Ptr;
    typedef std::unique_ptr<const CMABO> ConstPtr;

    CMABO() : NIPES(){}
    CMABO(const settings::ParametersMapPtr& param) : NIPES(param){}

    void epoch();
    void init_next_pop();
    bool update(const Environment::Ptr&);

protected:
    obs_fct_t _compute_obs;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    BOLearner::Ptr learner;
};

}


#endif //CMABO_HPP
