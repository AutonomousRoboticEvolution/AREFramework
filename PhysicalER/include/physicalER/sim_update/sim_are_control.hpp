#ifndef ARE_CONTROL_HPP
#define ARE_CONTROL_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include "simulatedER/Morphology_CPPNMatrix.h"
#include "ARE/NNParamGenome.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/nn2/NN2Control.hpp"
#include "ARE/ProtomatrixGenome.hpp"
#include "physicalER/pi_communication.hpp"
#include <zmq.hpp>
#include "nn2/mlp.hpp"
#include "nn2/elman.hpp"
#include "nn2/rnn.hpp"
#include "nn2/fcp.hpp"

namespace are {
namespace sim {

using neuron_t = nn2::Neuron<nn2::PfWSum<double>,nn2::AfSigmoidSigned<std::vector<double>>>;
using connection_t = nn2::Connection<double>;
using ffnn_t = nn2::Mlp<neuron_t,connection_t>;
using elman_t = nn2::Elman<neuron_t,connection_t>;
using rnn_t = nn2::Rnn<neuron_t,connection_t>;
using fcp_t = nn2::Fcp<neuron_t,connection_t>;

/**
 * @brief class Individual meant to be running on a Raspberry Pi using NN implementation of nn2 module
 */
class AREIndividual : public Individual
{
public:

    typedef std::shared_ptr<AREIndividual> Ptr;
    typedef std::shared_ptr<const AREIndividual> ConstPtr;

    AREIndividual() : Individual(){}
    AREIndividual(const Genome::Ptr& morph_gen,const NNParamGenome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen){}
    AREIndividual(const AREIndividual& ind) :
        Individual(ind),
        energy_cost(ind.energy_cost)
    {}

    ~AREIndividual(){
    }

    void init() override{
        isEval = false;
        Individual::init();
    }

    Individual::Ptr clone() override {
        return std::make_shared<AREIndividual>(*this);
    }

    void update(double sim_time) override;

    std::string to_string();
    void from_string(const std::string&);

    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & objectives;
        arch & ctrlGenome;
        arch & nb_input;
        arch & nb_output;
        arch & nb_hidden;
        arch & nn_type;
        arch & energy_cost;
    }

    //GETTERS & SETTERS
    double get_energy_cost(){return energy_cost;}

    void set_trajectory(const std::vector<waypoint> &traj){trajectory = traj;}
    const std::vector<waypoint>& get_trajectory(){return trajectory;}



protected:
    void createMorphology() override;
    void createController() override;



    std::vector<waypoint> trajectory;

    int nb_input;
    int nb_output;
    int nb_hidden;
    int nn_type;

    double energy_cost=0;
};

class AREControl{
public:
    typedef std::unique_ptr<AREControl> Ptr;
    typedef std::unique_ptr<const AREControl> ConstPtr;

    AREControl(){}
    AREControl(const AREIndividual& ind , std::string stringListOfOrgans , settings::ParametersMapPtr parameters);
    AREControl(const AREControl& ctrl):
            controller(ctrl.controller),
            _max_eval_time(ctrl._max_eval_time),
            _time_step(ctrl._time_step),
            _sent_finish_mess(ctrl._sent_finish_mess),
            cameraInputToNN(ctrl.cameraInputToNN),
            number_of_sensors(ctrl.number_of_sensors),
            number_of_wheels(ctrl.number_of_wheels),
            number_of_joints(ctrl.number_of_joints),
            _is_ready(ctrl._is_ready){}


    int exec( zmq::socket_t& socket, float sim_time);

    bool is_ready(){return _is_ready;}
    void set_ready(bool r = true){_is_ready = r;}
    AREIndividual &access_controller(){return controller;}

    void send_fitness(zmq::socket_t &socket);
    void send_trajectory(zmq::socket_t &socket);

private:
    AREIndividual controller;
    u_int32_t _max_eval_time ; // millieconds
    float _time_step ; // milliseconds
    bool _sent_finish_mess = false;

    bool cameraInputToNN;

    int number_of_sensors=0;
    int number_of_wheels=0;
    int number_of_joints=0;
    bool _is_ready = false;

};

}//pi
}//are

#endif //PIMIO_CONTROL_HPP
