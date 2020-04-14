#ifndef BOCMAES_HPP
#define BOCMAES_HPP

#include <ARE/EA.h>
#include "Novelty.hpp"
#include <libcmaes/cmaes.h>
#include "NNGenome.hpp"
#include "BOCMAESIndividual.hpp"
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

    static std::map<int,std::string> scriterias;

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
        _stopcriteria.set_criteria_active(cmaes::FTARGET,false);//[Success] The objective function target value has been reached
    }

    ~customCMAStrategy() {}

    //custom stop criteria
    bool pop_stagnation();
    bool best_sol_stagnation();
    bool reach_ftarget();

    dMat ask()
    {
        return ipop_cmaes_t::ask();
    }

    void eval(const dMat &candidates = dMat(0,0),
              const dMat &phenocandidates=dMat(0,0));

    void tell();
    bool stop();

    void capture_best_solution(cmaes::CMASolutions& best_run){
        ipop_cmaes_t::capture_best_solution(best_run);
    }

    void reset_search_state();


    void lambda_inc(){
        ipop_cmaes_t::lambda_inc();
    }

    void set_population(const std::vector<Individual::Ptr>& pop){_pop = pop;}
    void set_elitist_restart(bool er){elitist_restart = er;}
    void set_length_of_stagnation(int los){len_of_stag = los;}
    void set_novelty_ratio(double nr){novelty_ratio = nr; start_novelty_ratio = nr;}
    void set_novelty_decr(double nd){novelty_decr = nd;}

    bool have_reached_ftarget(){return reached_ft;}

    std::vector<std::string> log_stopping_criterias;


private:
    std::vector<Individual::Ptr> _pop;
    bool elitist_restart = false;
    std::vector<double> best_fitnesses;
    int len_of_stag = 5;
    double novelty_ratio = 0;
    double start_novelty_ratio = 0;
    double novelty_decr = 0.05;

    double best_fitness();

    bool reached_ft = false;

};

class BOCMAES : public EA
{
public:
    BOCMAES() : EA(){}
    BOCMAES(const settings::ParametersMapPtr& param) : EA(param){}
    ~BOCMAES(){
        cmaStrategy.reset();
    }

    void init();
    void epoch();
    void init_next_pop();
    bool update(const Environment::Ptr&);

    void cmaes_init_pop();
    void cmaes_next_pop();
    void cmaes_epoch();
    void bo_epoch();

    void setObjectives(size_t indIdx, const std::vector<double> &objectives);

    bool is_finish();

    bool restarted(){return !cmaStrategy->log_stopping_criterias.empty();}
    std::string pop_stopping_criterias(){
        std::string res = cmaStrategy->log_stopping_criterias.back();
        cmaStrategy->log_stopping_criterias.pop_back();
        return res;
    }


    const std::vector<Eigen::VectorXd> &get_archive(){return archive;}

private:
    customCMAStrategy::Ptr cmaStrategy;
    size_t currentIndIndex;
    cmaes::CMASolutions best_run;
    bool _is_finish = false;

    bool boIsActive = true;

    std::vector<Eigen::VectorXd> archive;

    std::vector<float> currentFitnesses;
    std::vector<Eigen::VectorXd> observations;
    std::vector<Eigen::VectorXd> samples;
    std::vector<double> final_position;
};





}

#endif //BOCMAES_HPP
