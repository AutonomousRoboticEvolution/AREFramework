#ifndef IPOP_CMAES_HPP
#define IPOP_CMAES_HPP

#include <memory>
#include <sstream>
#include <libcmaes/cmaes.h>
#include <ARE/Individual.h>
#include <ARE/NNParamGenome.hpp>

namespace cma = libcmaes;
using geno_pheno_t = cma::GenoPheno<cma::pwqBoundStrategy>;
using cov_update_t = cma::CovarianceUpdate;
using ipop_cmaes_t = cma::IPOPCMAStrategy<cov_update_t,geno_pheno_t>;
using eostrat_t = cma::ESOStrategy<cma::CMAParameters<geno_pheno_t>,cma::CMASolutions,cma::CMAStopCriteria<geno_pheno_t>>;

namespace are {

/**
 * @brief IPOP-CMAES : Increasing POPulation Covariance Matrix Evolutionary Strategy with custom restart mechanism and possibility to add novelty as special objective
 * WARNING : This learner minimize fitnesses between 0 and 1.
 *
 */
class IPOPCMAStrategy : public ipop_cmaes_t
{
private:
//    cma::FitFunc emptyObj = [](const double*,const int&) -> double{};


public:

    typedef struct individual_t{
        std::vector<double> genome;
        std::vector<double> objectives; //this objectives are the oposite of the reward and novelty use here (reward = 1 - obj0) Those are the same objective values than the one obtain in the EA class.
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

    IPOPCMAStrategy(){}

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

    double learning_progress(){/*TODO*/}

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

    std::string to_string();
    void from_string(const std::string&);
    void from_file(const std::string&);

    std::string print_info();

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & _solutions;
        arch & _parameters;
        arch & novelty_ratio;
        arch & number_of_eval;
        arch & _niter;
        arch & _nevals;
    }

    int nbr_iter() const{return _niter;}

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
    double starting_fitness = 0;
    int number_of_eval = 0;

    double best_fitness(std::vector<double> &);

    void _set_starting_fitness();

    bool reached_ft = false;

};

template<class ea_t>
class CMAESStateLog : public Logging
{
public:
    CMAESStateLog() : Logging(){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
        int gen = ea->get_generation();
        std::stringstream filename;
        filename << "cmaes_state_" << gen;
        std::ofstream ofs;
        if(!openOLogFile(ofs,filename.str()))
            return;
        const IPOPCMAStrategy::Ptr &ipop_cma = static_cast<ea_t*>(ea.get())->get_cma_strat();

        ofs << ipop_cma->to_string();
        ofs.close();
    }
    void loadLog(const std::string& logFile){}
};

}//are

#endif //IPOP_CMAES_HPP
