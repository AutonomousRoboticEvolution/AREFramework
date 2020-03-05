#ifndef CMAES_HPP
#define CMAES_HPP

#include <ARE/EA.h>
#include <libcmaes/cmaes.h>
#include "NNGenome.hpp"
#include "CMAESIndividual.hpp"
#include "mazeEnv.h"

namespace cmaes = libcmaes;

namespace are{

class customCMAStrategy : public cmaes::CMAStrategy<cmaes::CovarianceUpdate>
{
private:
//    cmaes::FitFunc emptyObj = [](const double*,const int&) -> double{};

public:

    typedef std::shared_ptr<customCMAStrategy> Ptr;

    customCMAStrategy(){}
    customCMAStrategy(cmaes::FitFunc func,cmaes::CMAParameters<> &parameters)
        :CMAStrategy<cmaes::CovarianceUpdate>(func,parameters)
    {}

    ~customCMAStrategy() {}

    dMat ask()
    {
        return CMAStrategy<cmaes::CovarianceUpdate>::ask();
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
        cmaes::CMAStrategy<cmaes::CovarianceUpdate>::tell();
    }

    bool stop()
    {
        return cmaes::CMAStrategy<cmaes::CovarianceUpdate>::stop();
    }

    void set_population(const std::vector<Individual::Ptr>& pop){_pop = pop;}

private:
    std::vector<Individual::Ptr> _pop;

};

class CMAES : public EA
{
public:
    CMAES() : EA(){}
    CMAES(const settings::ParametersMapPtr& param) : EA(param),
        cmaStrategy(new customCMAStrategy){}
    ~CMAES(){
        cmaStrategy.reset();
    }

    void init();
    void epoch();
    bool update(const Environment::Ptr&);

    void setObjectives(size_t indIdx, const std::vector<double> &objectives);

private:
    customCMAStrategy::Ptr cmaStrategy;
    int currentIndIndex;

};





}

#endif //CMAES_HPP
