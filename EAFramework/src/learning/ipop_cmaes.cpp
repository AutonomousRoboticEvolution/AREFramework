#include "ARE/learning/ipop_cmaes.hpp"

using namespace are;

std::map<int,std::string> IPOPCMAStrategy::scriterias = {{cma::CONT,"OK"},
                                                         {cma::AUTOMAXITER,"The automatically set maximal number of iterations per run has been reached"},
                                                         {cma::TOLHISTFUN,"[Success] The optimization has converged"},
                                                         {cma::EQUALFUNVALS,"[Partial Success] The objective function values are the same over too many iterations, check the formulation of your objective function"},
                                                         {cma::TOLX,"[Partial Success] All components of covariance matrix are very small (e.g. < 1e-12)"},
                                                         {cma::TOLUPSIGMA,"[Error] Mismatch between step size increase and decrease of all eigenvalues in covariance matrix. Try to restart the optimization."},
                                                         {cma::STAGNATION,"[Partial Success] Median of newest values is not smaller than the median of older values"},
                                                         {cma::CONDITIONCOV,"[Error] The covariance matrix's condition numfber exceeds 1e14. Check out the formulation of your problem"},
                                                         {cma::NOEFFECTAXIS,"[Partial Success] Mean remains constant along search axes"},
                                                         {cma::NOEFFECTCOOR,"[Partial Success] Mean remains constant in coordinates"},
                                                         {cma::MAXFEVALS,"The maximum number of function evaluations allowed for optimization has been reached"},
                                                         {cma::MAXITER,"The maximum number of iterations specified for optimization has been reached"},
                                                         {cma::FTARGET,"[Success] The objective function target value has been reached"}};

bool IPOPCMAStrategy::reach_ftarget(){
    cma::LOG_IF(cma::INFO,!_parameters.quiet()) << "Best fitness : "  << best_fitnesses.back() << std::endl;

    if (_parameters.get_ftarget() != std::numeric_limits<double>::infinity())
    {
        if (best_fitnesses.back() <= _parameters.get_ftarget())
        {
            std::stringstream sstr;
            sstr << "stopping criteria fTarget => fvalue=" << best_fitnesses.back() << " / ftarget=" << _parameters.get_ftarget();
            log_stopping_criterias.push_back(sstr.str());
            cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
            return true;
        }
    }
    return false;
}

bool IPOPCMAStrategy::pop_desc_stagnation(){
    std::vector<Eigen::VectorXd> descriptors;
    for (const auto& ind: _pop){
        Eigen::VectorXd desc;
        misc::stdvect_to_eigenvect(ind.descriptor,desc);
        descriptors.push_back(desc);
    }

    Eigen::VectorXd mean = Eigen::VectorXd::Zero(descriptors[0].rows());
    for(Eigen::VectorXd desc : descriptors){
        mean += desc;
    }
    mean = mean/static_cast<double>(descriptors.size());

    Eigen::VectorXd stddev = Eigen::VectorXd::Zero(descriptors[0].rows());
    for(Eigen::VectorXd desc : descriptors)
        stddev += (desc - mean).cwiseProduct(desc - mean);

    for(int i = 0; i < stddev.rows(); i++)
        stddev(i) = sqrt(stddev(i)/static_cast<double>(descriptors.size() - 1));

    bool stop = true;
    float sum = 0;
    for(int i = 0; i < stddev.rows(); i++){
        stop = stop && stddev(i) <= pop_stag_thres;
        sum += stddev(i);
    }
    sum = sum/static_cast<float>(stddev.rows());

    if(stop){
        std::stringstream sstr;
        sstr << "Stopping : standard deviation of the descriptor population is smaller than " << pop_stag_thres << " : " << sum;
        log_stopping_criterias.push_back(sstr.str());
        cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
    }
    return stop;
}

bool IPOPCMAStrategy::pop_fit_stagnation(){
    std::vector<double> fvalues;
    for(const auto& ind : _pop)
        fvalues.push_back(ind.objectives[0]);

    double mean=0.0;
    for(double fv : fvalues)
        mean+=fv;
    mean = mean/static_cast<double>(fvalues.size());

    double stddev = 0.0;
    for(double fv : fvalues)
        stddev+= (fv - mean)*(fv-mean);

    stddev = sqrt(stddev/static_cast<double>(fvalues.size()-1));
    cma::LOG_IF(cma::INFO,!_parameters.quiet()) << "pop standard deviation : " << stddev << std::endl;

    if(stddev <= pop_stag_thres){
        std::stringstream sstr;
        sstr << "Stopping : standard deviation of the population is smaller than " << pop_stag_thres << " : " << stddev;
        log_stopping_criterias.push_back(sstr.str());
        cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
        return true;
    }else return false;
}

bool IPOPCMAStrategy::mean_fitness_stagnation(){
    if(mean_fitnesses.size() < len_of_stag)
        return false;
    double mean = 0.0;
    for(size_t i = mean_fitnesses.size() - len_of_stag
        ; i < mean_fitnesses.size(); i++){
        mean += mean_fitnesses[i];
    }
    mean = mean/static_cast<float>(len_of_stag);
    double stddev = 0.0;
    for(size_t i = mean_fitnesses.size() - len_of_stag
        ; i < mean_fitnesses.size(); i++){
        stddev += (mean_fitnesses[i] - mean)*(mean_fitnesses[i] - mean);
    }
    stddev = sqrt(stddev/static_cast<float>(len_of_stag-1));

    if(stddev <= gen_stag_thres){
        std::stringstream sstr;
        sstr << "Stopping : standard deviation of the last " << len_of_stag <<  " mean fitnesses is smaller than " << gen_stag_thres << " : " << stddev;
        log_stopping_criterias.push_back(sstr.str());
        cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
        return true;
    }else return false;
}


bool IPOPCMAStrategy::best_sol_stagnation(){
    if(best_fitnesses.size() < len_of_stag)
        return false;
    double mean = 0.0;
    for(size_t i = best_fitnesses.size() - len_of_stag
        ; i < best_fitnesses.size(); i++){
        mean += best_fitnesses[i];
    }
    mean = mean/static_cast<float>(len_of_stag);
    double stddev = 0.0;
    for(size_t i = best_fitnesses.size() - len_of_stag
        ; i < best_fitnesses.size(); i++){
        stddev += (best_fitnesses[i] - mean)*(best_fitnesses[i] - mean);
    }
    stddev = sqrt(stddev/static_cast<float>(len_of_stag-1));
    if(stddev <= gen_stag_thres){
        std::stringstream sstr;
        sstr << "Stopping : standard deviation of the last " << len_of_stag <<  " best fitnesses is smaller than " << gen_stag_thres << " : " << stddev;
        log_stopping_criterias.push_back(sstr.str());
        cma::LOG_IF(cma::INFO,!_parameters.quiet()) << sstr.str() << std::endl;
        return true;
    }else return false;
}

void IPOPCMAStrategy::eval(const dMat &candidates, const dMat &phenocandidates){
    // custom eval.
    _solutions.candidates().clear();
    for (size_t r=0;r<_pop.size();r++)
    {
        std::vector<double> genome = _pop[r].genome;
        dVec x(genome.size());
        for(size_t i = 0; i < genome.size(); i++)
            x(i) = genome[i];

        double reward = 0;
        double fvalue = 0;
        if(start_novelty_ratio > 0){
            size_t i = 0;
            for(; i < _pop[r].objectives.size() - 1; i++)
                reward += 1 - _pop[r].objectives[i];

            reward = reward/static_cast<double>(_pop[r].objectives.size() - 1);

            fvalue = (1-novelty_ratio)*reward
                    + novelty_ratio*(1-_pop[r].objectives[i]);
        }
        else{
            for(const double& obj :  _pop[r].objectives)
                reward += 1 - obj;

            fvalue = reward/_pop[r].objectives.size();

        }

        _solutions.candidates().push_back(cma::Candidate(fvalue,x));
    }
    update_fevals(candidates.cols());
}

void IPOPCMAStrategy::tell()
{
    ipop_cmaes_t::tell();
    individual_t best_sample;
    best_fitnesses.push_back(best_fitness(best_sample));
    if(initial_fitness == 0)
        initial_fitness = 1 - best_fitnesses[0];
    if(novelty_ratio > 0)
        novelty_ratio -= novelty_decr;
    if(best_seen_solution.objectives.empty() || best_fitnesses.back() < 1-best_seen_solution.objectives[0])
        best_seen_solution = best_sample;
    inc_iter();
}

bool IPOPCMAStrategy::stop()
{
    reached_ft = reach_ftarget();
    bool ipop_stop = ipop_cmaes_t::stop();
    bool pop_stag = pop_desc_stagnation();
    bool sol_stag = false;
    if(len_of_stag > 0){
        if(fit_stagnation_method == "best")
            sol_stag = best_sol_stagnation();
        else if(fit_stagnation_method == "mean")
            sol_stag = mean_fitness_stagnation();
    }

    if(ipop_stop){
        log_stopping_criterias.push_back(scriterias[_solutions.run_status()]);
    }
    return  pop_stag || sol_stag  || ipop_stop;
}

void IPOPCMAStrategy::reset_search_state()
{
    if(elitist_restart)
        _parameters.set_x0(best_seen_solution.genome,best_seen_solution.genome);

    ipop_cmaes_t::reset_search_state();
    novelty_ratio = start_novelty_ratio;
    best_fitnesses.clear();
    mean_fitnesses.clear();
}

double IPOPCMAStrategy::best_fitness(individual_t &best_sample){
    double bf = 1.;
    double mean = 0;
    for(const auto& ind : _pop){
        mean+=ind.objectives[0];
        if(bf > 1 - ind.objectives[0]){
            bf = 1 - ind.objectives[0];
            best_sample = ind;
        }
    }
    mean_fitnesses.push_back(mean/static_cast<double>(_pop.size()));
    return bf;
}

std::string IPOPCMAStrategy::to_string(){
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<libcmaes::CMAParameters<geno_pheno_t>>();
    oarch.register_type<libcmaes::RankedCandidate>();
    oarch << *this;
    return sstream.str();
}

void IPOPCMAStrategy::from_string(const std::string & str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<libcmaes::CMAParameters<geno_pheno_t>>();
    iarch.register_type<libcmaes::RankedCandidate>();
    iarch >> *this;
}

void IPOPCMAStrategy::from_file(const std::string &filename){
    std::ifstream ifs(filename);
    if(!ifs){
        std::cerr << "unable to open : " << filename << std::endl;
        return;
    }
    boost::archive::text_iarchive iarch(ifs);
    iarch.register_type<libcmaes::CMAParameters<geno_pheno_t>>();
    iarch.register_type<libcmaes::RankedCandidate>();
    iarch >> *this;
    ifs.close();
}

std::string IPOPCMAStrategy::print_info(){
    std::stringstream sstr;
    sstr << "INFO - CMA-ES" <<
            " / dim = " << _parameters.dim() <<
            " / lambda = " << _parameters.lambda() <<
            " / sigma0 = " << _solutions.sigma() <<
            " / novelty_ratio = " << novelty_ratio;

    return sstr.str();
}


