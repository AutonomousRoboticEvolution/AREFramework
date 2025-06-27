#ifndef VISU_POP_HPP
#define VISU_POP_HPP

#include <filesystem>
#include "ARE/EA.h"
#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/NNParamGenome.hpp"
// #include "simulatedER/are_morphology.hpp"
#include "simulatedER/FixedMorphology.hpp"
// #include "simulatedER/nn2/NN2Individual.hpp"
// #include "simulatedER/Morphology_CPPNMatrix.h"
// #include "ARE/nn2/NN2Settings.hpp"
#include "ARE/Settings.h"
#include "mlp.hpp"


namespace are {

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
        arch & rollout;
    }

    std::string to_string() const override;
    void from_string(const std::string &str) override;

    void set_trajectory(const std::vector<waypoint>& traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}

private:
    void createMorphology() override;
    void createController() override;
    std::vector<waypoint> trajectory;
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
    void load_per_gen_ind(int indIdx, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);
    void load_per_id(int id, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files);

};

}
#endif //VISU_POP_HPP
