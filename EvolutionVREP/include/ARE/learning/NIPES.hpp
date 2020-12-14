#ifndef NIPES_HPP
#define NIPES_HPP

#include <ARE/EA.h>
#include "ARE/learning/Novelty.hpp"
#include <libcmaes/cmaes.h>
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/mazeEnv.h"

namespace cma = libcmaes;
using geno_pheno_t = cma::GenoPheno<cma::pwqBoundStrategy>;
using cov_update_t = cma::CovarianceUpdate;
using ipop_cmaes_t = cma::IPOPCMAStrategy<cov_update_t,geno_pheno_t>;
using eostrat_t = cma::ESOStrategy<cma::CMAParameters<geno_pheno_t>,cma::CMASolutions,cma::CMAStopCriteria<geno_pheno_t>>;

namespace are{

class IPOPCMAStrategy : public ipop_cmaes_t
{
private:
//    cma::FitFunc emptyObj = [](const double*,const int&) -> double{};


public:

    typedef struct individual_t{
        std::vector<double> genome;
        std::vector<double> objectives;
        std::vector<double> descriptor;

        template<class archive>
        void serialize(archive &arch, const unsigned int v)
        {
            arch & genome;
            arch & objectives;
            arch & descriptor;
        }


    }individual_t;

    static std::map<int,std::string> scriterias;

    typedef std::shared_ptr<IPOPCMAStrategy> Ptr;
    typedef std::shared_ptr<const IPOPCMAStrategy> ConstPtr;

    IPOPCMAStrategy(cma::FitFunc func,cma::CMAParameters<geno_pheno_t> &parameters)
        :ipop_cmaes_t(func,parameters)
    {
        _stopcriteria.set_criteria_active(cma::AUTOMAXITER,false); //The automatically set maximal number of iterations per run has been reached
        _stopcriteria.set_criteria_active(cma::TOLHISTFUN,false);  //[Success] The optimization has converged
        _stopcriteria.set_criteria_active(cma::EQUALFUNVALS,false); //[Partial Success] The objective function values are the same over too many iterations, check the formulation of your objective function
        _stopcriteria.set_criteria_active(cma::TOLX,false);//[Partial Success] All components of covariance matrix are very small (e.g. < 1e-12)
        _stopcriteria.set_criteria_active(cma::TOLUPSIGMA,false);//[Error] Mismatch between step size increase and decrease of all eigenvalues in covariance matrix. Try to restart the optimization.
        _stopcriteria.set_criteria_active(cma::STAGNATION,false);//[Partial Success] Median of newest values is not smaller than the median of older values
        _stopcriteria.set_criteria_active(cma::CONDITIONCOV,false);//[Error] The covariance matrix's condition number exceeds 1e14. Check out the formulation of your problem
        _stopcriteria.set_criteria_active(cma::NOEFFECTAXIS,false);//[Partial Success] Mean remains constant along search axes
        _stopcriteria.set_criteria_active(cma::NOEFFECTCOOR,false);//[Partial Success] Mean remains constant in coordinates
        _stopcriteria.set_criteria_active(cma::MAXFEVALS,false);//The maximum number of function evaluations allowed for optimization has been reached
        _stopcriteria.set_criteria_active(cma::MAXITER,false);//The maximum number of iterations specified for optimization has been reached
        _stopcriteria.set_criteria_active(cma::FTARGET,false);//[Success] The objective function target value has been reached
    }

    ~IPOPCMAStrategy() {}

    //custom stop criteria
    bool pop_desc_stagnation();
    bool pop_fit_stagnation();
    bool best_sol_stagnation();
    bool reach_ftarget();

    dMat ask()
    {
        _pop.clear();
        return ipop_cmaes_t::ask();
    }

    void eval(const dMat &candidates = dMat(0,0),
              const dMat &phenocandidates=dMat(0,0));

    void tell();
    bool stop();

    void capture_best_solution(cma::CMASolutions& best_run){
        ipop_cmaes_t::capture_best_solution(best_run);
    }

    void reset_search_state();


    void lambda_inc(){
        ipop_cmaes_t::lambda_inc();
    }

    void add_individual(const individual_t &ind){_pop.push_back(ind);}
    void set_population(const std::vector<Individual::Ptr>& pop);
    void set_population(const std::vector<individual_t>& pop){_pop = pop;}
    const std::vector<individual_t> &get_population(){return _pop;}
    std::vector<individual_t> &access_population(){return _pop;}
    void set_elitist_restart(bool er){elitist_restart = er;}
    void set_length_of_stagnation(int los){len_of_stag = los;}
    void set_novelty_ratio(double nr){novelty_ratio = nr; start_novelty_ratio = nr;}
    void set_novelty_decr(double nd){novelty_decr = nd;}
    void set_pop_stag_thres(float pst){pop_stag_thres = pst;}
    const std::pair<double,std::vector<double>> &get_best_seen_solution(){return best_seen_solution;}

    bool have_reached_ftarget(){return reached_ft;}

    std::vector<std::string> log_stopping_criterias;


private:
    std::vector<individual_t> _pop;
    bool elitist_restart = false;
    std::vector<double> best_fitnesses;
    std::pair<double,std::vector<double>> best_seen_solution;
    int len_of_stag = 20;
    float pop_stag_thres = 0.05;
    double novelty_ratio = 0;
    double start_novelty_ratio = 0;
    double novelty_decr = 0.05;

    double best_fitness(std::vector<double> &);

    bool reached_ft = false;

};

class NIPES : public EA
{
public:
    NIPES() : EA(){}
    NIPES(const settings::ParametersMapPtr& param) : EA(param){}
    ~NIPES(){
        cmaStrategy.reset();
    }

    void init() override;
    void epoch() override;
    void init_next_pop() override;
    bool update(const Environment::Ptr&) override;

    bool is_finish() override;
    bool finish_eval() override;

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

