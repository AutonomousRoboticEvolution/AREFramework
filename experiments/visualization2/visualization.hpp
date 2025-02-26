#ifndef VISU_POP_HPP
#define VISU_POP_HPP

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/EA.h"
#include "ARE/Individual.h"
#include "simulatedER/Morphology_CPPNMatrix.h"

#include "ARE/Settings.h"


namespace are {

using CPPNMorph = sim::Morphology_CPPNMatrix;

class ManualDesign: public Genome{
public:
    ManualDesign(std::vector<std::vector<int>> lov){
        list_of_voxels = lov;
    }
    ManualDesign(const ManualDesign& md):
        list_of_voxels(md.list_of_voxels){}
    Genome::Ptr clone() const override{return nullptr;}
    void init() override{}
    void mutate() override{}
    std::string to_string() const override{return "";}
    void from_string(const std::string &) override{}

    std::vector<std::vector<int>> list_of_voxels;
};

class VisuInd : public Individual
{
public:
    VisuInd() : Individual(){}
    VisuInd(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen)
        : Individual(morph_gen,ctrl_gen)
    {
    }
    VisuInd(const VisuInd& ind) :
        Individual(ind)
    {}
    Individual::Ptr clone() override{
        return std::make_shared<VisuInd>(*this);
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

    std::string to_string() const override;
    void from_string(const std::string &str) override;

    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}

private:
    void createMorphology() override;
    void createController() override;
    std::vector<waypoint> trajectory;
    double sum_ctrl_freq = 0;


};

class Visu : public EA
{
public:
    Visu() : EA(){}
    Visu(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~Visu(){
    }

    void init() override;
    bool is_finish() override;
    bool update(const Environment::Ptr&) override;
    void load_per_id(int id, std::string &morph_gen_file, std::string &ctrl_gen_file);

};

}
#endif //VISU_POP_HPP
