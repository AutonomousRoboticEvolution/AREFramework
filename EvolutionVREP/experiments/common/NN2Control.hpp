#ifndef NN2CONTROL_H
#define NN2CONTROL_H

#include "ARE/Control.h"
#include <multineat/NeuralNetwork.h>
#include <eigen3/Eigen/Core>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

namespace are {

template<class nn_t>
class NN2Control : public Control
{
public:
    NN2Control() : Control(){}
    NN2Control(const NN2Control& ctrl) : Control(ctrl){}

    Control::Ptr clone() const override {
        return std::make_shared<NN2Control>(*this);
    }

    std::vector<double> update(const std::vector<double> &sensorValues){
        double maxVelocity = settings::getParameter<settings::Double>(parameters,"#maxVelocity").value;
        bool useInternalBias = settings::getParameter<settings::Boolean>(parameters,"#UseInternalBias").value;
        double noiselvl = settings::getParameter<settings::Double>(parameters,"#noiseLevel").value;
        boost::mt19937 rng(randomNum->getSeed());
        std::vector<double> inputs = sensorValues;
        if(noiselvl > 0.0){
            for(double &sv : inputs){
                boost::normal_distribution<> normal(sv,noiselvl);
                sv = normal(rng);
            }
        }


        nn.step(inputs);
        std::vector<double> output = nn.outf();

        if(noiselvl > 0.0){
            for(double &o : output){
                boost::normal_distribution<> normal(o,noiselvl);
                o = normal(rng);
            }
        }

        for(double &o : output)
            o = o*maxVelocity;

        return output;
    }


    void set_randonNum(const misc::RandNum::Ptr& rn){randomNum = rn;}

    nn_t nn;

};

}

#endif //NN2CONTROL_H
