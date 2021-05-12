#include "NNControl.h"

using namespace are;

std::vector<double> NNControl::update(const std::vector<double> &sensorValues)
{
    nn.Flush();
    std::vector<double> values;
    for(int i = 0; i < sensorValues.size(); i++)
        values.push_back(sensorValues[i]);
//    for(int i = values.size(); i < nn.m_num_inputs; i++) //if sensor value is less than input size, add 0
//        values.push_back(0.0);
    nn.Input(values);
    unsigned int depth = 5;
    for(unsigned int i = 0; i < depth; i++){//why it needs to be activated for 3 times
        nn.Activate();
    }
    std::vector<double> output = nn.Output();
//    if(noiselvl > 0.0){
//        for(double &o : output){
//            boost::normal_distribution<> normal(o,noiselvl);
//            o = normal(rng);
//        }
//    }
    for(double &o : output)
        o = o*2.5;  //maxVelocity
    return output;
    //nn.ActivateUseInternalBias();
}

void NNControl::saveControl(FILE *file) {

    nn.Save(file);

}
