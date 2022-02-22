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
    void load_robot_matrix();

    NEAT::Genome loadInd(short int genomeID);
    std::vector<int> listInds();

private:
    std::unique_ptr<NEAT::Population> morph_population;

    NEAT::Parameters params;

    std::vector<Eigen::VectorXd> archive;

    std::vector<std::vector<std::vector<double>>> robot_skeleton_matrix;
    std::vector<std::vector<std::vector<double>>> robot_angle_matrix;
    std::vector<std::vector<std::vector<double>>> robot_wheel_matrix;
    std::vector<std::vector<std::vector<double>>> robot_sensor_matrix;
    std::vector<std::vector<std::vector<double>>> robot_caster_matrix;
    std::vector<std::vector<std::vector<double>>> robot_joint_matrix;

protected:
    NEAT::RNG rng;
    int currentIndIndex;

};

}

#endif //MATRIXEVOLUTION_H
