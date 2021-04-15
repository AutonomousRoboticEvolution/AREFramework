#ifndef SA_NIPES_HPP
#define SA_NIPES_HPP

#include <ARE/EA.h>
#include "ARE/learning/Novelty.hpp"
#include <libcmaes/cmaes.h>
#include <libcmaes/surrogatestrategy.h>
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "ARE/nn2/NN2Individual.hpp"
#include "ARE/mazeEnv.h"

namespace cma = libcmaes;
using geno_pheno_t = cma::GenoPheno<cma::pwqBoundStrategy>;
using cov_update_t = cma::CovarianceUpdate;
using sa_cmaes_t = cma::ACMSurrogateStrategy<cma::IPOPCMAStrategy,cov_update_t,geno_pheno_t>;
using eostrat_t = cma::ESOStrategy<cma::CMAParameters<geno_pheno_t>,cma::CMASolutions,cma::CMAStopCriteria<geno_pheno_t>>;

namespace are{

class SACMAStrategy : public sa_cmaes_t
{
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

    typedef std::shared_ptr<SACMAStrategy> Ptr;
    typedef std::shared_ptr<const SACMAStrategy> ConstPtr;

    SACMAStrategy(cma::FitFunc func,cma::CMAParameters<geno_pheno_t> &parameters)
        :sa_cmaes_t(func,parameters)
    {

    }

    ~SACMAStrategy() {}

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
        sa_cmaes_t::capture_best_solution(best_run);
    }

    void reset_search_state();


    void lambda_inc(){
        sa_cmaes_t::lambda_inc();
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

#endif //SA_NIPES_HPP

