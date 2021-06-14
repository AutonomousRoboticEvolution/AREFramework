#ifndef ROBUSTNESS_TEST_HPP
#define ROBUSTNESS_TEST_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "ARE/CPPNGenome.h"
#include "simulatedER/Morphology_CPPNMatrix.h"



namespace are {
using CPPNMorph = sim::Morphology_CPPNMatrix;

class RobustInd : public Individual
{
public:

    RobustInd() : Individual(){}
    RobustInd(const CPPNGenome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {
    }
    RobustInd(const RobustInd& ind) :
        Individual(ind)
    {}
    Individual::Ptr clone() override{
        return std::make_shared<RobustInd>(*this);
    };
    void update(double delta_time) override;
    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}
    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}

    std::string to_string() override;
    void from_string(const std::string &str) override;

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & morphGenome;
        arch & final_position;
        arch & trajectory;
    }

private:
    void createMorphology() override;
    void createController() override;
    std::vector<double> final_position;
    std::vector<waypoint> trajectory;
};

class RobustnessTest : public EA
{
public:
    using list_files_pair_t = std::vector<std::pair<std::string,std::string>>;

    RobustnessTest() : EA(){}
    RobustnessTest(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~RobustnessTest(){
    }

    void init() override;
//    void init_next_pop() override;
    bool update(const Environment::Ptr &) override;
    void setObjectives(size_t indIdx, const std::vector<double> &objectives) override;

private:
    int currentIndIndex;

    void load_gen_files(list_files_pair_t& list_gen_files, const std::string& folder, int gen_to_load);
};

}
#endif //ROBUSTNESS_TEST_HPP
