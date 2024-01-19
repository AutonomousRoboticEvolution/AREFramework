#ifndef MEIM_HPP
#define MEIM_HPP

#include <ARE/EA.h>
#include <ARE/nn2/NN2CPPNGenome.hpp>
#include <simulatedER/Morphology_CPPNMatrix.h>#
#include "homeokinesis_controller.hpp"


namespace are{

typedef struct genome_t{
    NN2CPPNGenome morph_genome;
    hk::Homeokinesis controller;
    std::vector<double> objectives;
} genome_t;


using CPPNMorph = sim::Morphology_CPPNMatrix;

class MEIMIndividual : public Individual
{
public:
    MEIMIndividual() : Individual(){}
    MEIMIndividual(const NN2CPPNGenome::Ptr& morph_gen,const EmptyGenome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {}
    MEIMIndividual(const MEIMIndividual& ind):
        Individual(ind){}


    Individual::Ptr clone() override {
        return std::make_shared<MEIMIndividual>(*this);
    }

    void update(double delta_time) override;

    std::string to_string() override;
    void from_string(const std::string &) override;

    void set_init_position(const std::vector<double>& ip){init_position = ip;}


private:
    void createMorphology() override;
    void createController() override;

    int individual_id;
    std::vector<double> init_position;


};

class MEIM: public EA
{
public:
    typedef std::unique_ptr<MEIM> Ptr;
    typedef std::unique_ptr<const MEIM> ConstPtr;

    MEIM() : EA(){}
    MEIM(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param);

    void init() override;
    void init_next_pop() override;
    bool update(const Environment::Ptr &) override;
    bool finish_eval(const Environment::Ptr &) override;
    bool is_finish() override;

    void setObjectives(size_t index, const std::vector<double> &objs) override;

    void fill_ind_to_eval(std::vector<int> &ind_to_eval) override;



private:
   std::vector<int> newly_evaluated;
   void reproduction();
   std::vector<genome_t> parents_pool;

   NN2CPPNGenome best_of_subset(const std::vector<genome_t>);

};
}

#endif //MEIM_HPP
