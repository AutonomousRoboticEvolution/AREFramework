#ifndef MorphNeuro_H
#define MorphNeuro_H

#include "ARE/EA.h"
#include "ARE/CPPNGenome.h"
#include "CPPNIndividual.h"
#include "ARE/Morphology_CPPNMatrix.h"

#include "eigen3/Eigen/Core"

#include "ARE/learning/Novelty.hpp"

namespace are {

    class MorphNeuro : public EA
    {
    public:
        MorphNeuro() : EA(){}
        MorphNeuro(const settings::ParametersMapPtr& param) : EA(param){}
        ~MorphNeuro() override {}

        void init() override;
        void initPopulation();
        void epoch() override;
        bool is_finish() override;
        void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;
        void init_next_pop() override;
        bool update(const Environment::Ptr & env);
        NEAT::Genome loadInd(short int genomeID);
        NEAT::Genome loadContrInd(short int genomeID);
        std::vector<int> listInds();
        //bool finish_eval() override;

    private:
        std::unique_ptr<NEAT::Population> morph_population;
        std::unique_ptr<NEAT::Population> contr_population;
        std::unique_ptr<NEAT::Population> contr_population_rand;
        std::unique_ptr<NEAT::Population> inner_morph_population;
        std::unique_ptr<NEAT::Population> inner_contr_population;
        std::unique_ptr<NEAT::Population> multi_morph_population;
        std::unique_ptr<NEAT::Population> multi_contr_population;
        std::unique_ptr<NEAT::Population> inner_multi_contr_population;

        NEAT::Parameters params;
        NEAT::Parameters paramsCtrl;
        NEAT::Parameters inner_params;
        NEAT::Parameters inner_paramsCtrl;
        NEAT::Parameters multi_params;
        NEAT::Parameters multi_paramsCtrl;

        std::vector<Eigen::VectorXd> archive;

    protected:
        NEAT::RNG rng;
        int currentIndIndex = 0;
        int reevaluated = 0;

    };

    class LC_NSMS : public EA
    {
    public:
        LC_NSMS() : EA(){}
        LC_NSMS(const settings::ParametersMapPtr& param) : EA(param){}
        ~LC_NSMS() override {}

        void init() override;
        void initPopulation();
        void epoch() override;
        bool is_finish() override;
        void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;
        void init_next_pop() override;

        NEAT::Genome loadInd(short int genomeID);
        std::vector<int> listInds();

    private:
        std::unique_ptr<NEAT::Population> morph_population;

        NEAT::Parameters params;

        std::vector<Eigen::VectorXd> archive;

    protected:
        NEAT::RNG rng;
        int currentIndIndex;

    };

}

#endif //MorphNeuro_H
