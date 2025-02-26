#ifndef HOMEOKINESIS_CONTROLLER_HPP
#define HOMEOKINESIS_CONTROLLER_HPP

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/QR>
#include <ARE/Control.h>
#include <ARE/misc/utilities.h>
#include <algorithm>

namespace are{

namespace hk{


/// configuration object for Sox controller. Use Sox::getDefaultConf().
typedef struct config_t {
  double initFeedbackStrength;  ///< initial strength of sensor to motor connection
  bool   useExtendedModel;      ///< if true, the extended model (S matrix) is used
  /// if true the controller can be taught see teachable interface
  bool   useTeaching;
  /// # of steps the sensors are averaged (1 means no averaging)
  size_t    steps4Averaging;
  /// # of steps the motor values are delayed (1 means no delay)
  size_t    steps4Delay;
  bool   someInternalParams;    ///< if true only some internal parameters are exported
  bool   onlyMainParameters;    ///< if true only some configurable parameters are exported
  size_t buffersize; ///< the maximum size of the memory buffer

  double epsA;
  double epsC;

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
        _set_default_config();
        init(nb_inputs,nb_outputs);
    }
    Homeokinesis(const Homeokinesis& ctrl):
        Control(ctrl),
        _nbr_inputs(ctrl._nbr_inputs),
        _nbr_outputs(ctrl._nbr_outputs),
        x_buffer(ctrl.x_buffer),
        y_buffer(ctrl.y_buffer),
        _conf(ctrl._conf),
        causeaware(ctrl.causeaware),
        sense(ctrl.sense),
        creativity(ctrl.creativity),
        harmony(ctrl.harmony),
        pseudo(ctrl.pseudo),
        damping(ctrl.damping),
        gamma(ctrl.gamma),
        loga(ctrl.loga)
       // intern_isTeaching(ctrl.intern_isTeaching)
    {
        A = ctrl.A;
        S = ctrl.S;
        C = ctrl.C;
        b = ctrl.b;
        h = ctrl.h;
        L = ctrl.L;
        v_avg = ctrl.v_avg;
        R = ctrl.R;
        A_native = ctrl.A_native;
        C_native = ctrl.C_native;
        y_teaching = ctrl.y_teaching;
        x = ctrl.x;
        x_smooth = ctrl.x_smooth;
    }

    void init(int nb_inputs, int nb_outputs);

    Control::Ptr clone() const override{
        return std::make_shared<Homeokinesis>(*this);
    }

    std::vector<double> update(const std::vector<double> &sensorValues) override;


    void step(const Matrix &x, Matrix &y);

    void learn();

    void set_epsA(double e){_conf.epsA = e;}
    void set_epsC(double e){_conf.epsC = e;}
    void add_noise(double e);
    void set_creativity(double e){creativity = e;}


private:

    void _set_default_config();

    int _nbr_inputs;
    int _nbr_outputs;

    Matrix A; //Forward model
    Matrix S; //extenstion of forward model to integrate the current state
    Matrix C; //Controller
    Matrix b; //bias for the forward model
    Matrix h; //bias for the controller
    Matrix L;
    Matrix v_avg;
    Matrix R; //Sensitivity?
    Matrix A_native;
    Matrix C_native;
    Matrix y_teaching;
    Matrix x;
    Matrix x_smooth;
    std::vector<Matrix> x_buffer;
    std::vector<Matrix> y_buffer;

    config_t _conf;
    int t=0;

    double causeaware = 0.01;
    double sense = 1;
    double creativity = 0.1;
    double harmony = 0.1;
    int pseudo = 0;
    double damping = 0.00001;
    double gamma = 0.01;          // teaching strength
    bool loga = true;
 //   bool intern_isTeaching = false;

    inline void _tanh_diff(Matrix& m){
        m = m.array().tanh();
        m = m.array()*m.array();
        m = 1.0f - m.array();
    }

    /// function that clips the second argument to the interval [-first,first]
    static double clip(double r, double x){
      return std::min(std::max(x,-r),r);
    }
    /// calculates the inverse the argument (useful for Matrix::map)
    static double one_over(double x){
      return 1/x;
    }

    template<typename Fct>
    Matrix _matrix_map(const Matrix &M, Fct f){
        Matrix res = M;
        for(size_t i = 0; i < res.array().size(); i++)
            res.array()(i) = f(res.array()(i));
        return res;
    }
};

}//hk

}//are


#endif //HOMEOKINESIS_CONTROLLER_HPP

