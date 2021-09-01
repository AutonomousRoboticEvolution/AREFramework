#ifndef PHYSICAL_MORPHO_EVO_HPP
#define PHYSICAL_MORPHO_EVO_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <multineat/Population.h>
#include "ARE/EA.h"
#include "ARE/Settings.h"
#include "ARE/learning/Novelty.hpp"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/CPPNGenome.h"

namespace are{

class DummyEnv : public Environment
{
public:
    void init() override {}
    std::vector<double> fitnessFunction(const Individual::Ptr &ind) {return {0};}
    void update_info() override {}
};

class PMEIndividual : public Individual
{
public:
    typedef std::shared_ptr<PMEIndividual> Ptr;
    typedef std::shared_ptr<const PMEIndividual> ConstPtr;

    PMEIndividual() : Individual(){}
    PMEIndividual(const CPPNGenome::Ptr& morph_gen,const EmptyGenome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    PMEIndividual(const PMEIndividual& ind) :
        Individual(ind)
    {}

    Individual::Ptr clone() override
    {return std::make_shared<PMEIndividual>(*this);}

    void update(double delta_time) override {}

    void create_morphology(){
        createMorphology();
    }

    const Eigen::VectorXd &get_morphDesc() const {return morphDesc;}

private:
    void createMorphology() override;
    void createController() override{}

    NEAT::NeuralNetwork cppn;
    Eigen::VectorXd morphDesc;
    std::vector<bool> testRes;

};

class PhysicalMorphoEvo : public EA
{
public:
    typedef std::shared_ptr<PhysicalMorphoEvo> Ptr;
    typedef std::shared_ptr<const PhysicalMorphoEvo> ConstPtr;

    PhysicalMorphoEvo(){}
    PhysicalMorphoEvo(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    void init() override;
    void init_next_pop() override;

    void load_data_for_generate() override{}
    void write_data_for_generate() override;
private:
    NEAT::Parameters params;
    std::unique_ptr<NEAT::Population> morph_population;

};

}//are

#endif
