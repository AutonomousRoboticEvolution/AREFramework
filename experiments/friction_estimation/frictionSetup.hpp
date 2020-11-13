#ifndef FRICTION_SETUP_H
#define FRICTION_SETUP_H

#include <iostream>
#include "simulatedER/VirtualEnvironment.hpp"
#include "ARE/Individual.h"
#include "simulatedER/Morphology.h"
#include "ARE/EA.h"
#include "ARE/Logging.h"

namespace are {

class CubeMorphology : public sim::Morphology
{
public:
    CubeMorphology(const settings::ParametersMapPtr &param) : sim::Morphology(param){}

    void loadModel();
    void create() override{}
    void createAtPosition(float x, float y, float z) override;
    void updateFrictionCoeff();
    void command(const std::vector<double> &sensorValues) override{}
    std::vector<double> update() override{}
    are::Morphology::Ptr clone() const override{}
};

class FricInd : public Individual
{
public:
    FricInd(EmptyGenome::Ptr g1,EmptyGenome::Ptr g2) : Individual(g1,g2)
    {

    }


    Individual::Ptr clone() override{}
    void createController() override{}
    void createMorphology() override;
    void update(double delta_time) override{}

    void set_fric_coeff(double fc){fric_coeff = fc;}
private:
    double fric_coeff;
};

class FricEA : public EA
{
public:
    FricEA() : EA(){}
    FricEA(const settings::ParametersMapPtr& param) :  EA(param){}
    void init() override;
    void init_next_pop() override;
    bool is_finish() override;
    float get_friction_coeff(){return friction_coeff;}
private:
    float friction_coeff = 1;
};

class FrictionSetUp : public sim::VirtualEnvironment
{
public:

    typedef std::shared_ptr<FrictionSetUp> Ptr;
    typedef std::shared_ptr<const FrictionSetUp> ConstPtr;

    FrictionSetUp(){}

    ~FrictionSetUp(){}
    void init() override;

    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override;
    float updateEnv(float simulationTime, const sim::Morphology::Ptr &morph) override;
private:
    float robot_pos[3];
    float initial_pos[3];

};

class FricCoeffLog : public Logging
{
public:
    FricCoeffLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

} //are

#endif //FRICTION_SETUP_H
