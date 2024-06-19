#ifndef SMARTIES_HPP
#define SMARTIES_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/Settings.h"
#include "smarties.h"

namespace are {

class SmartInd : public Individual
{
public:
    SmartInd() : Individual(){}
    SmartInd(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {
    }
    SmartInd(const SmartInd& ind) :
        Individual(ind)
    {}
    Individual::Ptr clone() override{
        return std::make_shared<SmartInd>(*this);
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
    }

    std::string to_string() override;
    void from_string(const std::string &str) override;

    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}

private:
    void createMorphology() override;
    void createController() override;
    std::vector<waypoint> trajectory;
    int nb_sensors = 0;
    int nb_wheels = 0;
    int nb_joints = 0;
    std::unique_ptr<smarties::Communicator> comm;

};

class Smarties : public EA
{
public:
    Smarties() : EA(){}
    Smarties(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}

    void init() override;
    bool is_finish() override;
    bool update(const Environment::Ptr&) override;
    void load_per_gen_ind(int indIdx, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);
    void load_per_id(int id, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);

};

}
#endif //SMARTIES_HPP
