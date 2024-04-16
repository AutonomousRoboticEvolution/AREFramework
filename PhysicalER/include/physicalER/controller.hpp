#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "ARE/Individual.h"
#include "ARE/Environment.h"

namespace are{

namespace phy{


/**
 * @brief class Individual meant to be running on a Raspberry Pi using NN implementation of nn2 module
 */
class Controller : public Individual
{
public:

    typedef std::shared_ptr<Controller> Ptr;
    typedef std::shared_ptr<const Controller> ConstPtr;

    Controller() : Individual(){}
    Controller(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    Controller(const Controller& ind) :
        Individual(ind)
    {}

    void init() override{
        isEval = false;
        createController();
    }

//    Individual::Ptr clone() override {
//        return std::make_shared<Controller>(*this);
//    }

    void update(double delta_time) override;



    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
    }

    //GETTERS & SETTERS
    void set_inputs(const std::vector<double>& in){inputs=in;}
    const std::vector<double>& get_ouputs(){return outputs;}

    int get_number_of_inputs(){return nb_input;}
    int get_number_of_outputs(){return nb_output;}

    void set_trajectory(const std::vector<waypoint> &traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}

protected:
    void createMorphology() override{}

    std::vector<double> inputs;
    std::vector<double> outputs;

    std::vector<waypoint> trajectory;

    int nb_input;
    int nb_output;

};



}//phy
}//are
#endif //CONTROLLER_HPP
