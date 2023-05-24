#ifndef MULTI_TARGET_TEST_HPP
#define MULTI_TARGET_TEST_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/CPPNGenome.h"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/Settings.h"
#include "multiTargetMaze.hpp"

namespace are {

using CPPNMorph = sim::Morphology_CPPNMatrix;


class MultiTargetInd : public Individual
{
public:
    MultiTargetInd() : Individual(){}
    MultiTargetInd(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {
    }
    MultiTargetInd(const MultiTargetInd& ind) :
        Individual(ind),
        rewards(ind.rewards),
        trajectories(ind.trajectories),
        id(ind.id)
    {}
    Individual::Ptr clone() override{
        return std::make_shared<MultiTargetInd>(*this);
    };
    void update(double delta_time) override;


    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & morphGenome;
        arch & individual_id;
        arch & generation;
        arch & rewards;
        arch & trajectories;
        arch & id;
    }

    std::string to_string() override;
    void from_string(const std::string &str) override;


    void add_trajectory(const std::vector<waypoint>& traj){trajectories.push_back(traj);}
    const std::vector<std::vector<waypoint>>& get_trajectories(){return trajectories;}
    void set_final_position(const std::vector<double>& final_pos){final_position = final_pos;}
    const std::vector<double>& get_final_position(){return final_position;}

    int get_number_times_evaluated(){return rewards.size();}
    void reset_rewards(){rewards.clear();}
    void compute_fitness();
    void add_reward(double reward){rewards.push_back(reward);}
    const std::vector<double> &get_rewards(){return rewards;}
    void set_id(int gen,int ind){id = std::make_pair(gen,ind);}
    const std::pair<int,int> &get_id(){return id;}

private:
    void createMorphology() override;
    void createController() override;
    std::vector<std::vector<waypoint>> trajectories;
    std::vector<double> final_position;

    std::vector<double> rewards;
    std::pair<int,int> id;


};

class MultiTargetTest : public EA
{
public:
    MultiTargetTest() : EA(){}
    MultiTargetTest(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~MultiTargetTest(){
    }

    void init() override;
    bool is_finish() override;
    bool update(const Environment::Ptr&) override;

    void setObjectives(size_t indIndex, const std::vector<double> &objectives) override;

    void load_per_gen_ind(int indIdx, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);
    void load_per_id(int id, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);

};

class MultiTrajectoriesLog: public Logging
{
public:
    MultiTrajectoriesLog() : Logging(false){}
    void saveLog(EA::Ptr& ea);
    void loadLog(const std::string &file){}
};



class RewardsLog: public Logging
{
public:
    RewardsLog(const std::string &filename) : Logging(filename,false){}
    void saveLog(EA::Ptr& ea);
    void loadLog(const std::string &file){}
};

} //are



#endif //MULTI_TARGET_TEST_HPP
