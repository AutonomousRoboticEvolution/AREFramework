#ifndef HOMEOKINESIS_CONTROLLER_HPP
#define HOMEOKINESIS_CONTROLLER_HPP

#include <eigen3/Eigen/Core>
#include <ARE/Control.h>

/**
 * @brief The Homeokinesis implementation. self-organisation (sox) version from lpzrobot
 */
class Homeokinesis: public are::Control {
public:
    using Matrix = Eigen::MatrixXd;
    typedef std::shared_ptr<Homeokinesis> Ptr;
    typedef std::shared_ptr<const Homeokinesis> ConstPtr;

    Homeokinesis(): Control(){}
    Homeokinesis(int nb_inputs, int nb_outputs): Control(){
        init(nb_inputs,nb_outputs);
    }
    Homeokinesis(const Homeokinesis& ctrl): Control(ctrl){}

    void init(int nb_inputs, int nb_outputs){}

    Control::Ptr clone() const override;

    std::vector<double> update(const std::vector<double> &sensorValues) override;

    void learn();

private:
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

};


#endif //HOMEOKINESIS_CONTROLLER_HPP

