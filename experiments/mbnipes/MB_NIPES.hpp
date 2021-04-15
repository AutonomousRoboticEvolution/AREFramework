#ifndef MB_NIPES_HPP
#define MB_NIPES_HPP

#include "ARE/learning/NIPES.hpp"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/learning/BayesianOpt.hpp"

namespace are
{

using obs_fct_t = std::function<Eigen::VectorXd(const NN2Individual::Ptr &)>;

struct obs_fcts{
    /**
     * @brief Observation corresponding to the final position of the robot
     */
    static obs_fct_t final_position;
    static obs_fct_t reward;

};

class MB_NIPES : public NIPES
{

public:

    typedef std::unique_ptr<MB_NIPES> Ptr;
    typedef std::unique_ptr<const MB_NIPES> ConstPtr;

    MB_NIPES() : NIPES(){}
    MB_NIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}

    void init();
    void epoch();
    void init_next_pop();
    bool update(const Environment::Ptr&);
    Eigen::VectorXd getLastObs() const {return observations.back();}
    Eigen::VectorXd getLastSpl() const {return samples.back();}
    const BOLearner::Ptr &get_learner() const {return learner;}
protected:
    obs_fct_t _compute_obs;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    BOLearner::Ptr learner;
    std::vector<Individual::Ptr> virtual_pop;
};

}


#endif //MB_NIPES_HPP
