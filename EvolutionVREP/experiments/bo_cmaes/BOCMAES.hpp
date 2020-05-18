#ifndef BOCMAES_HPP
#define BOCMAES_HPP

#include <ARE/EA.h>
#include "Novelty.hpp"
#include <libcmaes/cmaes.h>
#include "NNGenome.hpp"
#include "BOCMAESIndividual.hpp"
#include "mazeEnv.h"
#include "NIPES.hpp"

namespace cmaes = libcmaes;
using geno_pheno_t = cmaes::GenoPheno<cmaes::pwqBoundStrategy>;
using cov_update_t = cmaes::CovarianceUpdate;
using ipop_cmaes_t = cmaes::IPOPCMAStrategy<cov_update_t,geno_pheno_t>;
using eostrat_t = cmaes::ESOStrategy<cmaes::CMAParameters<geno_pheno_t>,cmaes::CMASolutions,cmaes::CMAStopCriteria<geno_pheno_t>>;

namespace are{


class BOCMAES : public NIPES
{
public:
    BOCMAES() : NIPES(){}
    BOCMAES(const settings::ParametersMapPtr& param) : NIPES(param){}
    ~BOCMAES(){
        cmaStrategy.reset();
    }

    void init();
    void epoch();
    void init_next_pop();
    bool update(const Environment::Ptr&);

    void cmaes_init_pop();
    void cmaes_next_pop();
    void bo_epoch();

    bool restarted(){return !cmaStrategy->log_stopping_criterias.empty();}
    std::string pop_stopping_criterias(){
        std::string res = cmaStrategy->log_stopping_criterias.back();
        cmaStrategy->log_stopping_criterias.pop_back();
        return res;
    }

    Eigen::VectorXd getLastObs() const {return observations.back();}
    Eigen::VectorXd getLastSpl() const {return samples.back();}
    const std::vector<Eigen::VectorXd> &get_archive(){return archive;}

private:

    bool boIsActive = true;

    std::vector<float> currentFitnesses;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    std::vector<double> final_position;
};





}

#endif //BOCMAES_HPP
