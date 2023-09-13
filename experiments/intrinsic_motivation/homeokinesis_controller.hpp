#ifndef HOMEOKINESIS_CONTROLLER_HPP
#define HOMEOKINESIS_CONTROLLER_HPP

#include <eigen3/Eigen/Core>
#include <ARE/Control.h>
#include <ARE/utilities.h>


namespace are{

namespace hk{


/// configuration object for Sox controller. Use Sox::getDefaultConf().
typedef struct config_t {
  constexpr double initFeedbackStrength = ;  ///< initial strength of sensor to motor connection
  bool   useExtendedModel;      ///< if true, the extended model (S matrix) is used
  /// if true the controller can be taught see teachable interface
  bool   useTeaching;
  /// # of steps the sensors are averaged (1 means no averaging)
  int    steps4Averaging;
  /// # of steps the motor values are delayed (1 means no delay)
  int    steps4Delay;
  bool   someInternalParams;    ///< if true only some internal parameters are exported
  bool   onlyMainParameters;    ///< if true only some configurable parameters are exported
  int buffersize; ///< the maximum size of the memory buffer

  double factorS;             ///< factor for learning rate of S
  double factorb;             ///< factor for learning rate of b
  double factorh;             ///< factor for learning rate of h
}config_t;

/**
 * @brief The Homeokinesis implementation. self-organisation (sox) version from lpzrobot
 */
class Homeokinesis: public are::Control {
public:
    using Matrix = Eigen::MatrixXd;
    using Vector = Eigen::VectorXd;
    typedef std::shared_ptr<Homeokinesis> Ptr;
    typedef std::shared_ptr<const Homeokinesis> ConstPtr;

    Homeokinesis(): Control(){
        _set_default_config();
    }
    Homeokinesis(int nb_inputs, int nb_outputs): Control(){
        init(nb_inputs,nb_outputs);
        _set_default_config();
    }
    Homeokinesis(const Homeokinesis& ctrl): Control(ctrl){}

    void init(int nb_inputs, int nb_outputs){}

    Control::Ptr clone() const override;

    std::vector<double> update(const std::vector<double> &sensorValues) override;

    void step();

    void learn();


private:

    void _set_default_config();

    int _nbr_inputs;
    int _nnr_outputs;

    Matrix A; //Forward model
    Matrix S; //extenstion of forward model to integrate the current state
    Matrix C; //Controller
    Matrix b; //bias for the forward model
    Matrix h; //bias for the controller
    Matrix L;
    Matrix v_avg;
    Matrix R; //Sensitivity?

    config_t _conf;

};

}//hk

}//are


#endif //HOMEOKINESIS_CONTROLLER_HPP

