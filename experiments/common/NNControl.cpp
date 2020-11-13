#include "NNControl.h"

using namespace are;

std::vector<double> NNControl::update(const std::vector<double> &sensorValues)
{
    bool useInternalBias = settings::getParameter<settings::Boolean>(parameters,"#UseInternalBias").value;
    double maxVelocity = settings::getParameter<settings::Double>(parameters,"#maxVelocity").value;
    std::vector<double> inputs = sensorValues;
    nn.Input(inputs);
//    nn.Flush();

    //First activation to activate the hidden layer
    if(useInternalBias)
        nn.ActivateUseInternalBias();
    else nn.Activate();
//    //Have to activate a second time to activate the outputs
//    if(useInternalBias)
//        nn.ActivateUseInternalBias();
//    else nn.Activate();
    
    std::vector<double> output = nn.Output();

    for(double &o : output)
        o = o*maxVelocity;
    return output;
}

