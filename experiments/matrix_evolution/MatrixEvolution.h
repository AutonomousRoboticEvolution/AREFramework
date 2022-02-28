#ifndef MATRIXEVOLUTION_H
#define MATRIXEVOLUTION_H

#include "ARE/EA.h"
#include "CPPNIndividual.h"
#include "eigen3/Eigen/Core"
#include "multineat/Population.h"
#include "ARE/learning/Novelty.hpp"


namespace are {

class MATRIXEVOLUTION : public NSGA2<CPPNIndividual>
{
public:
    MATRIXEVOLUTION() : NSGA2<CPPNIndividual>(){}
    MATRIXEVOLUTION(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : NSGA2<CPPNIndividual>(rn, param){}
    ~MATRIXEVOLUTION() override {}

    void init() override;
    void initPopulation();
    void epoch() override;
    bool is_finish() override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;
    bool update(const Environment::Ptr&);
    std::vector<std::vector<double>> load_robot_matrix(std::string filepath);
    std::vector<std::vector<double>> mutate_matrix(std::vector<std::vector<double>> matrix_4d);
    std::vector<std::vector<double>> crossover_matrix(std::vector<std::vector<double>> first_parent_4d,std::vector<std::vector<double>> second_parent_4d);

    NEAT::Genome loadInd(short int genomeID);
    std::vector<int> listInds();

private:
    std::unique_ptr<NEAT::Population> morph_population;

    NEAT::Parameters params;

    std::vector<Eigen::VectorXd> archive;

    std::vector<std::vector<double>> first_parent_matrix_4d;
    std::vector<std::vector<double>> second_parent_matrix_4d;
    std::vector<std::vector<double>> child_matrix_4d;

protected:
    NEAT::RNG rng;
    int currentIndIndex;

};

}

#endif //MATRIXEVOLUTION_H
