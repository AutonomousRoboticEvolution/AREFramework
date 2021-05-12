#ifndef NNCONTROL_H
#define NNCONTROL_H

#include "ARE/Control.h"
#include <multineat/NeuralNetwork.h>
#include <eigen3/Eigen/Core>

namespace are {

class NNControl : public Control
{
public:
    NNControl() : Control(){}
    NNControl(const NEAT::NeuralNetwork &n) : nn(n){}
    NNControl(const NNControl& ctrl) : Control(ctrl), nn(ctrl.nn){}
    void saveControl(FILE *file) override;

    Control::Ptr clone() const override {
        return std::make_shared<NNControl>(*this);
    }

    std::vector<double> update(const std::vector<double> &sensorValues) override;


    NEAT::NeuralNetwork nn;

};

}

#endif //NNCONTROL_H
