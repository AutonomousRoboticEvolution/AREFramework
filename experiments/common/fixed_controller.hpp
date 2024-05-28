#ifndef FIXED_CONTROLLER_HPP
#define FIXED_CONTROLLER_HPP

#include "ARE/Control.h"
#include "ARE/nn2/NN2Control.hpp"

namespace are{

class FixedController: public Control
{
public:
    FixedController(): Control(){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<const settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<const settings::Double>(0));
    }
    FixedController(const FixedController& ctrl) : Control(ctrl), _nn(ctrl._nn){
        settings::defaults::parameters->emplace("#UseInternalBias",std::make_shared<const settings::Boolean>(1));
        settings::defaults::parameters->emplace("#noiseLevel",std::make_shared<const settings::Double>(0));
    }


    Control::Ptr clone() const override {
        return std::make_shared<FixedController>(*this);
    }

    std::vector<double> update(const std::vector<double> &sensorValues) override;

    void init_nn(int nb_input, int nb_hidden, int nb_output);
    void init_nn(int nb_input, int nb_hidden, int nb_output, double weight, double bias);

private:
    ffnn_t _nn;


};


}//are
#endif //FIXED_CONTROLLER_HPP
