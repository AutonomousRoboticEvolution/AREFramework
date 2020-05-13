#ifndef MFBO_HPP
#define MFBO_HPP

#include <boost/filesystem.hpp>

#include "ARE/EA.h"
#include "NNGenome.hpp"
#include "BOIndividual.h"
#include "testEnv.h"
#include "mazeEnv.h"


namespace are {

using obs_fct_t = std::function<Eigen::VectorXd(const Environment::Ptr)>;

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

namespace settings {

typedef enum obsType {
    FINAL_POS = 0,
    TRAJECTORY = 1,
    POS_TRAJ = 2
}obsType;

}

class MFBO : public EA
{
public:
    MFBO() : EA(){}
    MFBO(const settings::ParametersMapPtr& param) : EA(param){}
    ~MFBO() override {}

    void init() override;
    void epoch();
    bool update(const Environment::Ptr &);

    bool is_finish();

    void setObjectives(size_t indIndex, const std::vector<double> &objectives);
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

#endif //MFBO_HPP
