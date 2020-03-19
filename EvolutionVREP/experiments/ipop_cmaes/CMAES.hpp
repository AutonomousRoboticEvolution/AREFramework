#ifndef CMAES_HPP
#define CMAES_HPP

#include <ARE/EA.h>
#include <libcmaes/cmaes.h>
#include "NNGenome.hpp"
#include "CMAESIndividual.hpp"
#include "mazeEnv.h"

namespace cmaes = libcmaes;
using geno_pheno_t = cmaes::GenoPheno<cmaes::NoBoundStrategy>;
using cov_update_t = cmaes::CovarianceUpdate;
using ipop_cmaes_t = cmaes::IPOPCMAStrategy<cov_update_t,geno_pheno_t>;
using eostrat_t = cmaes::ESOStrategy<cmaes::CMAParameters<geno_pheno_t>,cmaes::CMASolutions,cmaes::CMAStopCriteria<geno_pheno_t>>;

namespace are{

class customCMAStrategy : public ipop_cmaes_t
{
private:
//    cmaes::FitFunc emptyObj = [](const double*,const int&) -> double{};


public:



    typedef std::shared_ptr<customCMAStrategy> Ptr;

    customCMAStrategy(cmaes::FitFunc func,cmaes::CMAParameters<> &parameters)
        :ipop_cmaes_t(func,parameters)
    {
        _stopcriteria.set_criteria_active(cmaes::AUTOMAXITER,true); //The automatically set maximal number of iterations per run has been reached
        _stopcriteria.set_criteria_active(cmaes::TOLHISTFUN,false);  //[Success] The optimization has converged
        _stopcriteria.set_criteria_active(cmaes::EQUALFUNVALS,true); //[Partial Success] The objective function values are the same over too many iterations, check the formulation of your objective function
        _stopcriteria.set_criteria_active(cmaes::TOLX,true);//[Partial Success] All components of covariance matrix are very small (e.g. < 1e-12)
        _stopcriteria.set_criteria_active(cmaes::TOLUPSIGMA,true);//[Error] Mismatch between step size increase and decrease of all eigenvalues in covariance matrix. Try to restart the optimization.
        _stopcriteria.set_criteria_active(cmaes::STAGNATION,true);//[Partial Success] Median of newest values is not smaller than the median of older values
        _stopcriteria.set_criteria_active(cmaes::CONDITIONCOV,true);//[Error] The covariance matrix's condition number exceeds 1e14. Check out the formulation of your problem
        _stopcriteria.set_criteria_active(cmaes::NOEFFECTAXIS,true);//[Partial Success] Mean remains constant along search axes
        _stopcriteria.set_criteria_active(cmaes::NOEFFECTCOOR,true);//[Partial Success] Mean remains constant in coordinates
        _stopcriteria.set_criteria_active(cmaes::MAXFEVALS,false);//The maximum number of function evaluations allowed for optimization has been reached
        _stopcriteria.set_criteria_active(cmaes::MAXITER,false);//The maximum number of iterations specified for optimization has been reached
        _stopcriteria.set_criteria_active(cmaes::FTARGET,true);//[Success] The objective function target value has been reached
    }

    ~customCMAStrategy() {}

    bool pop_stagnation();

    dMat ask()
    {
        return ipop_cmaes_t::ask();
    }

    void eval(const dMat &candidates = dMat(0,0),
              const dMat &phenocandidates=dMat(0,0))
    {
        // custom eval.
        for (int r=0;r<_pop.size();r++)
        {
            std::vector<double> genome = std::dynamic_pointer_cast<NNParamGenome>(_pop[r]->get_ctrl_genome())->get_full_genome();
            dVec x(genome.size());
            for(int i = 0; i < genome.size(); i++)
                x(i) = genome[i];

            _solutions.get_candidate(r).set_x(x);

            _solutions.get_candidate(r).set_fvalue(-_pop[r]->getObjectives()[0]);

        }
        update_fevals(candidates.cols());
    }

    void tell()
    {
        ipop_cmaes_t::tell();
    }

    bool stop()
    {
        std::cout << "FTarget " << _parameters.get_ftarget() << std::endl;
        return pop_stagnation() || ipop_cmaes_t::stop();
    }

    void capture_best_solution(cmaes::CMASolutions& best_run){
        ipop_cmaes_t::capture_best_solution(best_run);
    }

    void reset_search_state()
    {
        ipop_cmaes_t::reset_search_state();
        if(elitist_restart){
            _parameters.set_x0(_solutions.get_best_seen_candidate().get_x_dvec_ref());
        }

    }

    void lambda_inc(){
        ipop_cmaes_t::lambda_inc();
    }

    void set_population(const std::vector<Individual::Ptr>& pop){_pop = pop;}
    void set_elitist_restart(bool er){elitist_restart = er;}

private:
    std::vector<Individual::Ptr> _pop;
    bool elitist_restart = false;

};

class CMAES : public EA
{
public:
    CMAES() : EA(){}
    CMAES(const settings::ParametersMapPtr& param) : EA(param){}
    ~CMAES(){
        cmaStrategy.reset();
    }

    void init();
    void epoch();
    bool update(const Environment::Ptr&);

    void setObjectives(size_t indIdx, const std::vector<double> &objectives);

    bool is_finish();

private:
    customCMAStrategy::Ptr cmaStrategy;
    int currentIndIndex;
    cmaes::CMASolutions best_run;
    bool _is_finish = false;
    std::vector<Individual::Ptr> savedPop;
};





}

#endif //CMAES_HPP
