#ifndef NIPES_HPP
#define NIPES_HPP


#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

#include "simulatedER/mazeEnv.h"
#include "ARE/learning/ipop_cmaes.hpp"
#include "ARE/learning/Novelty.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "ARE/Settings.h"

namespace are{


class NIPES : public EA
{
public:
    NIPES() : EA(){}
    NIPES(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~NIPES(){
        cmaStrategy.reset();
    }

    void init() override;
    void epoch() override;
    void init_next_pop() override;
    bool update(const Environment::Ptr&) override;

    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;

    bool is_finish() override;
    bool finish_eval(const Environment::Ptr&) override;

    bool restarted(){return !cmaStrategy->log_stopping_criterias.empty();}
    std::string pop_stopping_criterias(){
        std::string res = cmaStrategy->log_stopping_criterias.back();
        cmaStrategy->log_stopping_criterias.pop_back();
        return res;
    }
    const std::vector<Eigen::VectorXd> &get_archive(){return archive;}

protected:
    IPOPCMAStrategy::Ptr cmaStrategy;
    cma::CMASolutions best_run;
    bool _is_finish = false;
    std::vector<Eigen::VectorXd> archive;
    int reevaluated = 0;
};

}

#endif //NIPES_HPP


