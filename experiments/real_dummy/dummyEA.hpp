#ifndef DUMMY_EA_HPP
#define DUMMY_EA_HPP

#include "ARE/EA.h"

namespace are {

class DummyEnv : public Environment
{
public:
    DummyEnv() : Environment(){}

    void init() override{}
    void update_info(double time) override{}
    std::vector<double> fitnessFunction(const Individual::Ptr &ind) override{return std::vector<double>(1,0);}
};

class DummyInd : public Individual
{
public:
    typedef std::shared_ptr<DummyInd> Ptr;
    typedef std::shared_ptr<const DummyInd> ConstPtr;

    DummyInd() : Individual(){}
    Individual::Ptr clone() override{}
    void init() override;
    void update(double delta_time) override;
    void createController() override{}
    void createMorphology() override{}
};

class DummyEA : public EA
{
public:
    DummyEA() : EA(){}
    DummyEA(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    void init() override;
};

}//are

#endif //DUMMY_EA_HPP
