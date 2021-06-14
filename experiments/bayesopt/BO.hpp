#ifndef BO_HPP
#define BO_HPP

#include <boost/filesystem.hpp>

#include "ARE/EA.h"
#include "BOIndividual.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "simulatedER/mazeEnv.h"


namespace are {

using obs_fct_t = std::function<Eigen::VectorXd(const BOIndividual::Ptr &)>;

struct obs_fcts{
    /**
     * @brief Observation corresponding to the final position of the robot
     */
    static obs_fct_t final_position;
    /**
     * @brief trajectory of positions and orientations of the robot
     */
    static obs_fct_t trajectory;

    /**
     * @brief trajectory of positions
     */
    static obs_fct_t pos_traj;
};


class BO : public EA
{
public:
    BO() : EA(){}
    BO(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~BO() override {}

    void init() override;
    void epoch();
    bool update(const Environment::Ptr &);

    bool is_finish();

    void setObjectives(size_t indIndex, const std::vector<double> &objectives) override;
    Eigen::VectorXd getLastObs() const {return observations.back();}
    Eigen::VectorXd getLastSpl() const {return samples.back();}

private:
    size_t currentIndIndex;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    NEAT::RNG rng;
    bool _is_finish = false;
    obs_fct_t _compute_observation;
};

}

#endif //BO_HPP

